import pathlib as p

def linePopSwap(line: str, columnsToPop: list|bool, columnsToSwap: list|bool) -> str:
    split_line = line.split(',')

    # Swap columns that need repositioning.
    swapAllColumns(split_line, columnsToSwap)

    # Pop columns that should be removed.
    popColumns(split_line, columnsToPop)

    # Rejoin into csv line (Seperated by, commas).
    new_line = ",".join(split_line)
    
    return new_line

# split one csv into two as well as popping columns and swapping columns
def csvSplitPopSwap(filepath: p.Path, d1: p.Path, d2: p.Path, columnsToPop: list|bool, columnsToSwap: list|bool) -> int:
    if not filepath.exists():
        return 0
    
    try:
        with open(filepath, "r") as file, open(d1, "w") as destination1, open(d2, "w") as destination2:

            file.readline() # Skip column names.

            i = 0
            for line in file:

                # "Pop Swap" the line.
                new_line = linePopSwap(line, columnsToPop, columnsToSwap)

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
    
def swapAllColumns(split_line: str, columnsToSwap: list):
    if columnsToSwap:
        for columns in columnsToSwap:
            swapColumns(split_line, columns[0], columns[1])

def swapColumns(columns, swap1, swap2):
    var = columns[swap1]
    columns[swap1] = columns[swap2]
    columns[swap2] = var

def popColumns(columns: list, columnsToPop: list|bool) -> None:
    if columnsToPop:
        for q in columnsToPop:
            columns.pop(q)

'''
dir = "world_happiness_data"
fp = p.Path(dir, "2015.csv")
nfp = p.Path(dir, "2015rr20c.csv")
ffc = [11, 4, 2, 1, 0]
sc = [ [9, 10] ]

csvPopSwap(fp, nfp, ffc, sc)

fp1 = p.Path(dir, "2020.csv")
nfp1 = p.Path(dir, "2020rr15c.csv")
ffc1 = [19, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 1, 0]
sc1 = False

csvPopSwap(fp1, nfp1, ffc1, sc1)
'''
def csvPopSwap(filepath: p.Path, newFilepath: p.Path, columnsToPop: list|bool, columnsToSwap: list|bool) -> int:
    try:
        with open(filepath, "r+") as file, open(newFilepath, "w+") as destination:

            file.readline() # Skip column names.

            # instead of buffering with file.read(*chunk_size*) and introducing complexity
            # we can buffer with 'for line in file'!
            for line in file:

                # "Pop Swap" the line where specified.
                new_line = linePopSwap(line, columnsToPop, columnsToSwap)

                # Write to destination file.
                destination.write(new_line)

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

    csvSplitPopSwap(fp, fp1, fp2, ctp, cts)


if __name__ == '__main__':
    main()