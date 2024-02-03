import datetime
import argparse
import pandas as pd
import networkx as nx
from rich.console import Console

parser = argparse.ArgumentParser()
parser.add_argument("dataset")
args = parser.parse_args()
console = Console()

console.rule("load graph from file")
df = pd.read_csv(args.dataset, sep=" ", names=["source", "target", "timestamp"], dtype=int)
df["timestamp"] = pd.to_datetime(df["timestamp"],unit="s").dt.date
df["timestamp"] = df.groupby("timestamp").ngroup()
console.log(df)
G = nx.from_pandas_edgelist(df)
G.graph["lifespan"] = len(df["timestamp"].unique())
console.log(f"Load {G} in [0,{G.graph['lifespan']})")


