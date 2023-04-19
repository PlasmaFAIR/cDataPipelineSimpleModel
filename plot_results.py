r"""
A simple plotting utility to view data generated using a local run::

    $ ./build/cSimpleModel data/local_data.csv
    $ pip install matplotlib pandas
    $ python3 plot_results.py
"""

from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv(Path(__file__).parent / "data_store" / "c_simple_model.csv")

for col in df.columns[1:]:
    plt.plot(df["time"], df[col], label=col)

plt.xlabel("Time /years")
plt.ylabel("Population fraction")
plt.legend()
plt.show()
