# cDataPipelineSimpleModel

Simple SEIRS model for C API for the FAIR data pipeline.

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

Note the URL on which the server is running, which should be something like
`http://127.0.0.1:8000`. Then, initialise the repository:

```
$ fair init
```

The default inputs will work in most cases, but you may need to set the 'Remote API'
using the URL reported by the previous command, i.e. `http://127.0.0.1:8000/api/`.
You may also need to set the registry token, which should be at
`/home/USERNAME/.fair/registry/token`, where `USERNAME` is your login name. Finally,
run the model using:

```
$ fair pull data/config.yaml
$ fair run data/config.yaml
```
