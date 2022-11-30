# cDataPipelineSimpleModel

Simple SEIRS model for C API for the FAIR data pipeline.

To run locally, install [`cDataPipeline`](https://github.com/PlasmaFAIR/cDataPipeline)
as follows:

```bash
$ cd cDataPipeline
$ cmake -B build
& sudo cmake --build build --target install
```

Then, compile the simple model:

```bash

$ cd cDataPipelineSimpleModel
$ cmake -B build
$ cmake --build build
```

Finally, run using the provided data:

```bash
$ ./build/bin/cSimpleModel data/local_data.csv
```

The produced data can be found in a new directory called `data_store`.
