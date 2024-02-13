import questionary
import os
import pandas as pd
import networkx as nx
from rich.progress import track

dataset_dir = "data/"
while not os.path.isdir(dataset_dir):
    dataset_dir = questionary.path("Where's the dataset folder?").ask()
os.chdir(dataset_dir)
print(f"Load dataset folder {dataset_dir}")

dataset = questionary.select(
    "Choose a dataset file",
    choices= list(os.listdir())
).unsafe_ask()
df = pd.read_csv(dataset,sep=" ",names=["source","target","timestamp"],dtype=int,comment='#')
print(df)

match questionary.select("Timestamp format",choices = ["default","unix"]).ask():
    case "unix":
        df["timestamp"] = pd.to_datetime(df["timestamp"],unit="s").dt.date        
        df["timestamp"] = df.groupby("timestamp").ngroup()
min_vert = min(min(df["source"]),min(df["target"]))
df["source"] -= min_vert
df["target"] -= min_vert
print(df)

G = nx.from_pandas_edgelist(df,edge_attr=True,create_using=nx.MultiGraph)
snapshots = []
lifespan = max(df["timestamp"])+1
for t in range(lifespan):
    snapshots.append(nx.from_pandas_edgelist(df[df["timestamp"]==t]))
print(f"Load {G} in [0, {lifespan})")

df.to_csv(f"data.txt" ,sep=' ',index=False,header=False)

operations = questionary.checkbox("What do you want?", choices=["temporal core"]).ask()
if "temporal core" in operations:
    k = int(questionary.text("input k", default="3", validate= lambda x: x.isdigit()).ask())
    with open(f"data_core.txt", "w") as file:
        file.write(f"{len(G.nodes)} {len(G.edges)} {lifespan}\n")
        file.write(f"{(lifespan+1)*lifespan//2}\n")
        for ts in track(range(lifespan)):
            projected_graph = nx.Graph()
            for te in range(ts,lifespan):
                projected_graph = nx.compose(projected_graph,snapshots[te])
                core = set(nx.k_core(projected_graph,k))
                file.write(f"{ts} {te} {len(core)} ")
                for v in core:
                    file.write(f"{v} ")
                file.write('\n')


