cmake_minimum_required (VERSION 3.8)

# Targets
foreach (TARGET Poseidon)
	add_subdirectory(${TARGET})
endforeach ()

