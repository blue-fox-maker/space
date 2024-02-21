import networkx as nx
import questionary 
import os
from rich.progress import track

dataset_dir = "data/"
while not os.path.isdir(dataset_dir):
    dataset_dir = questionary.path("Where's the dataset folder?").ask()
os.chdir(dataset_dir)
print(f"Load dataset folder {dataset_dir}")

num_vert = int(questionary.text("input number of verts").ask())
num_edge = int(questionary.text("input number of edges").ask())
num_time = int(questionary.text("input number of times").ask())

with open("data.txt","w") as file:
    for t in track(range(num_time)):
        for u,v in nx.fast_gnp_random_graph(num_vert,num_edge/num_time/num_vert/num_vert).edges():
            file.write(f"{u} {v} {t}\n")
    
