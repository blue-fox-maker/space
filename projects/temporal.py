import networkx as nx
import argparse
import pandas as pd
import itertools
import operator
import numpy as np
from collections import defaultdict
from rich.console import Console

parser = argparse.ArgumentParser()
parser.add_argument("dataset")
args = parser.parse_args()
console = Console()

df = pd.read_csv(args.dataset,sep=" ",names = ["timestamp", "source", "target"],dtype=int)    
G = nx.from_pandas_edgelist(df,edge_attr=True)
console.log(f"Load {G} in [0,{t_max})")
G.graph["lifespan"] = max(df["timestamp"])+1

def construct_phc(G:nx.Graph):
    nx.core_number(G)
    core_neighbor = np.array((G.graph["lifespan"], G.graph["lifespan"]), dtype=int)
    for ts in range(G.graph["lifespan"]):
        for t in reversed(range(ts+1,G.graph["lifespan"])):
            DelEdges
