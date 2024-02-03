import datetime
import argparse
import pandas as pd
import networkx as nx
from itertools import product
from rich.progress import track
from collections import defaultdict
from rich.console import Console
import numpy as np

parser = argparse.ArgumentParser()
parser.add_argument("dataset")
args = parser.parse_args()
console = Console()

df = pd.read_csv(args.dataset, sep=" ", names=["source","target","timestamp"],dtype=int)
df.sort_values("timestamp")
# df["timestamp"] = pd.to_datetime(df["timestamp"],unit="s").dt.date
# df["timestamp"] = df.groupby("timestamp").ngroup()
G = nx.from_pandas_edgelist(df,edge_attr=["timestamp"])
G.graph["lifespan"] = len(df["timestamp"].unique())
console.log(f"Load {G} in [0,{G.graph['lifespan']})")

def core_decomposition(G,k):
    lifespan = G.graph["lifespan"]
    k_core = np.ndarray((lifespan,lifespan),dtype=dict)
    for ts,te in track(list(product(range(lifespan),range(lifespan)))):
        if ts<=te:
            projected_graph = nx.subgraph_view(G,filter_edge = lambda u,v: ts<=G[u][v]["timestamp"]<=te)
            k_core[ts,te] = set(nx.k_core(projected_graph,k).nodes)
        # else:
            # k_core[ts,te] = []
    return k_core

def construct_phc(G,k_core):
    lifespan = G.graph["lifespan"]
    phc_index = np.ndarray(max(G.nodes)+1,dtype=list)
    for v in track(G.nodes):
        t1,t2 = 0,0
        phc_index[v] = []
        while t2<lifespan:
            while t2<lifespan and v not in k_core[t1,t2]:
                t2 = t2+1
            if t2 == lifespan:
                break
            phc_index[v]+= [t1,t2-1]
            while t1<=t2 and v in k_core[t1,t2]:
                t1 = t1+1
            t2 = max(t1,t2)
        phc_index[v]+=[t1,lifespan]
    return phc_index

def phc_dump_to_file(path,G,phc):
    with open(path,"w") as file:
        for v in track(sorted(G.nodes)):
            file.write(' '.join(map(str,[v]+phc[v]))+'\n')
        
k_core = core_decomposition(G,3)
phc_index = construct_phc(G,k_core)
path = "phc.txt"
phc_dump_to_file(path, G, phc_index)
console.log(f"write phc index of size {sum(map(lambda v: len(phc_index[v]),G.nodes))/2} to {path}")
