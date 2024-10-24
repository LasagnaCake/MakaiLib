ifdef openmp
export openmp
endif

ifdef debug-release
export debug-release
endif

ifdef no-buffers
export no-buffers
endif

ifdef gl-loader
export gl-loader
else
export gl-loader:=glad
endif

ifdef omp-threads
export omp-threads
else
export omp-threads:=128
endif

export o?=g
