# General exercise instructions

For most of the exercises, skeleton codes are provided both for
Fortran and C/C++ in the corresponding subdirectory. Some exercise
skeletons have sections marked with “TODO” for completing the
exercises. In addition, all of the 
exercises have exemplary full codes (that can be compiled and run) in the
`solutions` folder. Note that these are seldom the only or even the best way to
solve the problem.

The exercise material can be downloaded with the command

```
git clone https://github.com/csc-training/hybrid-openmp-mpi.git
```

If you have a GitHub account you can also **Fork** this repository and clone then your fork.

## Computing servers

Exercises can be carried out using the CSC's Mahti supercomputer. 
See [CSC User Documentation](https://docs.csc.fi/support/tutorials/mahti_quick/) for general instructions on using Mahti.

In case you have working parallel program development environment in your
laptop (Fortran or C compiler, MPI development library, etc.) you may also use
that. Note, however, that no support for installing MPI environment can be provided during the course.

Mahti can be accessed via ssh using the
provided username (`trainingxxx`) and password:
```
ssh -Y training000@mahti.csc.fi
```


For editing program source files you can use e.g. *nano* editor: 

```
nano prog.f90 &
```
(`^` in nano's shortcuts refer to **Ctrl** key, *i.e.* in order to save file and exit editor press `Ctrl+X`)
Also other popular editors (emacs, vim, gedit) are available.

## Disk areas

All the exercises in the supercomputers should be carried out in the
**scratch** disk area. The name of the scratch directory can be
queried with the command `csc-workspaces`. As the base directory is
shared between members of the project, you should create your own
directory:
```
cd /scratch/project_2000745
mkdir -p $USER
cd $USER
```


## Compilation

In Mahti, programs (also serial codes without MPI) can be compiled with the
`mpif90`, `mpicxx`, and `mpicc` wrapper commands, OpenMP is enabled with the 
`-fopenmp` flag:
```
mpif90 -o my_exe test.f90 -fopenmp
```
or
```
mpicxx -o my_exe test.cpp -fopenmp
```
or
```
mpicc -o my_exe test.c -fopenmp
```

When code uses MPI, the wrapper commands include automatically all the flags needed for building
MPI programs.

## Running in Mahti

In Mahti, programs need to be executed via the batch job system. The number of nodes is specified with `--nodes` (for most of the exercises you should use only a single node), number of MPI tasks **per node** with `--ntasks-per-node`, and number of cores reserved for threading with `--cpus-per-task`. The actual number of threads is specified with `OMP_NUM_THREADS` environment variable. Simple job running with 4 MPI tasks and 4 OpenMP threads per MPI task can be submitted with the following batch job script:
```
#!/bin/bash
#SBATCH --job-name=example
#SBATCH --account=project_2000745
#SBATCH --partition=medium
#SBATCH --reservation=training
#SBATCH --time=00:05:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=4

# Set the number of threads based on --cpus-per-task
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
srun my_exe
```

Save the script *e.g.* as `job.sh` and submit it with `sbatch job.sh`. 
The output of job will be in file `slurm-xxxxx.out`. You can check the status of your jobs with `squeue -u $USER` and kill possible hanging applications with
`scancel JOBID`.

The reservation `training` is available during the course days and it
is accessible only with the training user accounts.

## Running in local workstation

In most workstations, programs build with OpenMP use as many threads as there are CPU cores 
(note that this might include also "logical" cores with simultaneous multithreading). A pure OpenMP
program can be normally started with specific number of threads with
```bash
OMP_NUM_THREADS=4 ./my_exe
```
In most MPI implementations parallel program can be started with the `mpiexec` launcher, so hybrid program
using 2 MPI tasks and 4 threads would be run with
```
OMP_NUM_THREADS=4 mpiexec -n 2 ./my_exe
```

## Debugging

### Allinea DDT

The [Allinea DDT parallel debugger](https://docs.csc.fi/apps/ddt/) is available in CSC 
supercomputers. In order to use the debugger, build your code first with the `-g` flag. The DDT is
then enabled via the module system:

```bash
module load ddt
```

The debugger is run in an interactive session, and for proper
functioning the environment variable `SLURM_OVERLAP` needs to be set.

1. Set `SLURM_OVERLAP` and request Slurm allocation interactively:
```bash
export SLURM_OVERLAP=1
salloc --nodes=1 --ntasks-per-node=2 --cpus-per-task=4 --account=project_2000745 --partition=medium --reservation=training
```
2. Start the application under debugger
```bash
ddt srun ./buggy
```

By default, DDT sets the initial breakpoint at `MPI_Init`. For debugging plain OpenMP programs, set also the following environment variables before starting the debugger:

```
export ALLINEA_MPI_INIT=main
export ALLINEA_HOLD_MPI_INIT=1
```

For smoother GUI performance, we recommend using [NoMachine remote
desktop](https://docs.csc.fi/support/tutorials/nomachine-usage/) to
connect to Mahti.

### GDB

Pure OpenMP programs can be debugged also with GDB. See GDB [general documentation](https://www.gnu.org/software/gdb/documentation/)
and some [multithreading specific information](https://hpcbootcamp.readthedocs.io/en/latest/day2.html#gdb-and-openmp) for more details.

## Performance analysis with ScoreP / Scalasca

Start by loading `scorep` and `scalasca` modules:

```bash
module load scorep scalasca
```

Instrument the application by prepeding compile command with `scorep`:

```bash
scorep mpicc -o my_mpi_app my_mpi_code.c -fopenmp
```

Collect and create flat profile by prepending `srun` with `scan`:
```
...
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=4

module load scalasca
scan srun ./my_mpi_app
```

Scalasca analysis report explorer `square` does not work currently in
the CSC supercomputers, but the experiment directory can be copied to
local workstation for visual analysis:

(On local workstation)
```bash
rsync -r mahti.csc.fi:/path_to_rundir/scorep_my_mpi_app_8_sum .
```

The `scorep-score` command can be used also in the supercomputers to
estimate storage requirements before starting tracing:

```bash
scorep-score -r scorep_my_mpi_app_8_sum/profile.cubex
```

In order to collect and analyze the trace, add `-q` and `-t` options
to `scan`:

```bash
...
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=4

module load scalasca
scan -q -t srun ./my_mpi_app
```

The experiment directory containing the trace can now be copied to
local workstation for visual analysis:

```bash
rsync -r mahti.csc.fi:/path_to_rundir/scorep_my_mpi_app_8_trace .
```

On CSC supercomputers, one can use Intel Traceanalyzer for
investigating the trace (Traceanalyzer can read the `.otf2` produced
by ScoreP / Scalasca):

```bash
module load intel-itac
traceanalyzer &
```

Next, choose the "Open" dialog and select the `trace.otf2` file within
the experiment directory (e.g. `scorep_my_mpi_app_8_trace`). For smoother GUI
performance, we recommend using [NoMachine remote desktop](https://docs.csc.fi/support/tutorials/nomachine-usage/) 
to connect to Puhti.

More information about Scalasca can be found in [CSC User Documentation](https://docs.csc.fi/apps/scalasca/)





