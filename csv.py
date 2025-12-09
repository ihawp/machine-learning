import pathlib as p

def addRank(split_line: list, rank: int):
    split_line.insert(0, str(rank))

def linePopSwap(line: str, columnsToPop: list[int]|bool, columnsToSwap: list[list]|bool, rank: int|bool = False) -> str:
    split_line = line.split(',')

    # Swap columns that need repositioning.
    swapAllColumns(split_line, columnsToSwap)

    # Pop columns that should be removed.
    popColumns(split_line, columnsToPop)

    if rank: addRank(split_line, rank)

    # Rejoin into csv line (Seperated by, commas).
    new_line = ",".join(split_line)
    
    return new_line

# split one csv into two as well as popping columns and swapping columns
def csvSplitPopSwap(filepath: p.Path, d1: p.Path, d2: p.Path, columnsToPop: list[int]|bool, columnsToSwap: list[list]|bool, rank: int|bool = False) -> int:
    if not filepath.exists():
        return 0
    
    try:
        with open(filepath, "r") as file, open(d1, "w") as destination1, open(d2, "w") as destination2:

            file.readline() # Skip column names.

            i = 0
            for line in file:

                if rank: rank = i + 1

                # "Pop Swap" the line.
                new_line = linePopSwap(line, columnsToPop, columnsToSwap, rank)

                # Add to correct file.

                if i % 2 == 0:
                    destination1.write(new_line)
                else:
                    destination2.write(new_line)

                i += 1

        return 1

    except (FileNotFoundError):
        print("Unable to open the file " + filepath)
        return 0
    
def swapAllColumns(split_line: str, columnsToSwap: list[list]|bool):
    if columnsToSwap:
        for columns in columnsToSwap:
            swapColumns(split_line, columns[0], columns[1])

def swapColumns(columns: list[str], swap1: int, swap2: int):
    var = columns[swap1]
    columns[swap1] = columns[swap2]
    columns[swap2] = var

def popColumns(columns: list, columnsToPop: list[int]|bool) -> None:
    if columnsToPop:
        for q in columnsToPop:
            columns.pop(q)

def csvPopSwap(filepath: p.Path, newFilepath: p.Path, columnsToPop: list[int]|bool, columnsToSwap: list[list]|bool, rank: int|bool = False) -> int:
    try:
        with open(filepath, "r+") as file, open(newFilepath, "w+") as destination:

            file.readline() # Skip column names.

            # instead of buffering with file.read(*chunk_size*) and introducing complexity
            # we can buffer with 'for line in file'!
            i = 0
            for line in file:

                if rank: rank = i + 1

                # "Pop Swap" the line where specified.
                new_line = linePopSwap(line, columnsToPop, columnsToSwap, rank)

                # Write to destination file.
                destination.write(new_line)

                i += 1

        return 1

    except (FileNotFoundError, FileExistsError):
        print("There was an error when trying to open a file.")
        return 0
    except (IndexError):
        print("Oh gosh.")
        return 0

def main():

    dir = "world_happiness_data"

    fp = p.Path(dir, "2020.csv")
    fp1 = p.Path(dir, "2020a.csv")
    fp2 = p.Path(dir, "2020b.csv")
    ctp = [1, 0]
    cts = False
    rank = True
    
    csvSplitPopSwap(fp, fp1, fp2, ctp, cts, rank)

    fp = p.Path(dir, "2019.csv")
    fp1 = p.Path(dir, "2019a.csv")
    fp2 = p.Path(dir, "2019b.csv")
    ctp = [1]
    cts = False
    
    csvSplitPopSwap(fp, fp1, fp2, ctp, cts)

    fp = p.Path(dir, "2018.csv")
    fp1 = p.Path(dir, "2018a.csv")
    fp2 = p.Path(dir, "2018b.csv")
    ctp = [1]
    cts = False
    
    csvSplitPopSwap(fp, fp1, fp2, ctp, cts)

    fp = p.Path(dir, "2017.csv")
    fp1 = p.Path(dir, "2017a.csv")
    fp2 = p.Path(dir, "2017b.csv")
    ctp = [0]
    cts = False
        
    csvSplitPopSwap(fp, fp1, fp2, ctp, cts)

    fp = p.Path(dir, "2016.csv")
    fp1 = p.Path(dir, "2016a.csv")
    fp2 = p.Path(dir, "2016b.csv")
    ctp = [1, 0]
    cts = False
    
    csvSplitPopSwap(fp, fp1, fp2, ctp, cts)

    fp = p.Path(dir, "2015.csv")
    fp1 = p.Path(dir, "2015a.csv")
    fp2 = p.Path(dir, "2015b.csv")
    ctp = [1, 0]
    cts = False
    
    csvSplitPopSwap(fp, fp1, fp2, ctp, cts)


if __name__ == '__main__':
    main()