# Preface

This document describes how physical memory manager of ReaverOS (refered to as "PMM" from here) works internally, what 
it does at init, at page or continuous pages request and "page freed" calls.

# Overview of PMM design

Due to its nature requiring as much responsiveness as possible, PMM is per-domain part of the kernel. As long as each 
domain has own free memory, it just allocates it using per instance data structures (discussed in "General data structures"
paragraph). If any of domains runs out of memory, it uses IPIs to "borrow" memory for closest NUMA domain having free
memory. In case of entire system running out of memory, PMM forces kernel to panic.



# General data structures

PMM uses generally two different data structures to keep track of unallocated pages (and VMM keeps track of allocated ones,
see VMM documentation).