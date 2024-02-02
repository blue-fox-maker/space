import networkx as nx
import argparse
import pandas as pd
import itertools
import operator
from collections import defaultdict
from rich.console import Console

parser = argparse.ArgumentParser()
parser.add_argument("dataset")
args = parser.parse_args()
console = Console()

df = pd.read_csv(args.dataset,sep=" ",names = ["timestamp", "source", "target"],dtype=int)    
G = nx.from_pandas_edgelist(df,edge_attr=True)
t_min = 1
t_max = max(df["timestamp"])+1
console.log(f"Load {G} in [0,{t_max})")
G.graph["t_min"] = 1
G.graph["t_max"] = t_max
