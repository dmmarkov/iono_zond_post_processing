# IonoZondPostProcessing
This project was developed in North Caucasus Federal University(NCFU).

The main purpose of this software is post processing of data that was collected from Novatel GPStation-6. 

# Prerequisite software
* CMake
* Boost 1.58 (program_options, thread, filesystem, chrono, system, date_time, atomic)
* gcc

# Compiling
You should be able build IonoZondPostprocessing software with `build.sh`.

The script have some options:
```sh
--release: build release version with optimization(default Debug)
--cores: set gcc -j option(default 1)
```

# Running
All results after compilation will be stored in build directory.
Output application is `ionozondpostprocessing`.

Application have some help option:
```sh
Usage options:
  -h [ --help ]           print usage message
  --file-with-sources arg file that contains list of packed RAW_LOG_###.tar.gz
  --experiment arg        name of experiment that will be activated
  --tmp-dir arg           folder that will be used for caching of files
  --tmp-unpack-dir arg    folder that will be used for temporary unpacking of files
  --merge-output-dir arg  folder that will be used for output of mergedata in csv format
  --satellite-system arg  Name of satellite system that should be output (GPS/GLONASS)
  --satellite-id arg      ID of satellite that will be output GPS(1-32), GLONASS(1-24)
  --satellite-channel arg channel L1, L2
  --sleep arg             Delay between reads from stdin in milliseconds
```

IonoZondPostProcessing may read line by line from argument of option `--file-with-sources` or may read `stdin`.

For example you may start application with this command
```sh
(cd build && cat /mnt/data/RAW_DATA_767.tar.gz | tar -xzf - -i -O | ./ionozondpostprocessing --experiment=experiment20 --tmp-dir=. --tmp-unpack-dir=. --merge-output-dir=. --sleep=2 2>/dev/null 1>frequency.tec)
```
