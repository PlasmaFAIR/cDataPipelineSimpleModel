# cDataPipelineSimpleModel

Simple SEIRS model for C API for the FAIR data pipeline. The C API is a component of
the C++ API.

## Installation

From the top-level directory:

```bash
$ cmake -B build
$ cmake --build build
```

Building will be much faster if
[`cDataPipeline`](https://github.com/PlasmaFAIR/cDataPipeline) has been installed
on the user's system. Otherwise, `cDataPipeline` will be downloaded and built first.

## Run locally

```bash
$ ./build/cSimpleModel data/local_data.csv
$ # If the executable is not found here, try:
$ ./build/bin/cSimpleModel data/local_data.csv
```

The produced data can be found in a new directory called `data_store`.

## Run using FAIR Data Pipeline


First, install the [`fair` CLI](https://github.com/FAIRDataPipeline/FAIR-CLI).

```bash
$ pip install fair-cli
```

To set up the run, first start a local registry:

```bash
$ fair registry install
$ fair registry start
```

Note the location of the registry token reported here. If using default settings, this
should be at `/home/USERNAME/.fair/registry/token`. Then, initialise the repository:

```bash
$ fair init
```

Finally, run the model using:

```bash
$ fair pull data/config.yaml
$ fair run data/config.yaml
```

If you don't have access to the remote server, a local run can be performed using:

```bash
$ fair pull --local data/config.yaml
$ fair run --local data/config.yaml
```

You can also run in a dirty git repo using:

```bash
$ fair run --dirty data/config.yaml
$ # Without remote server access:
$ fair run --local --dirty data/config.yaml
```
