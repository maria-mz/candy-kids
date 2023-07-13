# Candy Kids

A multithreaded C program that explores the Producer Consumer problem. Factory threads insert
candies into a bounded buffer, and kid threads consume the candies. 

## How to run

The program accepts three arguments:

```bash
./candykids <#factories> <#kids> <#seconds>
```

\#factories:  Number of candy-factory threads to spawn.

\#kids:       Number of kid threads to spawn.

\#seconds:    Number of seconds to allow the factory threads to run for.

## Example

```bash
./candykids 5 3 2
```

Terminal output:
```c
Factory 0 ships candy & waits 3s
Factory 1 ships candy & waits 2s
Factory 2 ships candy & waits 1s
Factory 4 ships candy & waits 1s
Factory 3 ships candy & waits 3s
Factory 2 ships candy & waits 2s
Factory 4 ships candy & waits 2s
Time 1s
Factory 1 ships candy & waits 2s
Time 2s
Candy-factory 4 done
Candy-factory 3 done
Candy-factory 0 done
Candy-factory 2 done
Candy-factory 1 done
Factory#     #Made    #Eaten       Min Delay[ms]       Avg Delay[ms]       Max Delay[ms]
       0         1         1             0.18286             0.18286             0.18286
       1         2         2             0.04297             0.09729             0.15161
       2         2         2             0.01904             0.06189             0.10474
       3         1         1             0.01709             0.01709             0.01709
       4         2         2             0.01416             0.01440             0.01465

```
