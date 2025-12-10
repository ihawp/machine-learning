import pathlib as p

def thresholdColumn(filepath: p.Path, outputPath: p.Path, valuableColumn: int, splitter: float) -> bool:
    try:
        with open(filepath, "r") as f, open(outputPath, "w") as f1:
            for i, line in enumerate(f):
                split_line = line.split(",")

                check_value = float(split_line[valuableColumn])

                if check_value > splitter:
                    if i == 0:
                        f1.write("1")
                        continue
                    f1.write(",1")
                else:
                    if i == 0:
                        f1.write("0")
                        continue
                    f1.write(",0")

        return True
    except (FileNotFoundError, FileExistsError):
        print("Unable to find specified file.")
        return False
    except (FloatingPointError):
        print("Oh geez, a floating point error.")
        return False

def addRank(split_line: list, rank: int) -> None:
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
    
def swapAllColumns(split_line: str, columnsToSwap: list[list]|bool) -> None:
    if columnsToSwap:
        for columns in columnsToSwap:
            swapColumns(split_line, columns[0], columns[1])

def swapColumns(columns: list[str], swap1: int, swap2: int) -> None:
    var = columns[swap1]
    columns[swap1] = columns[swap2]
    columns[swap2] = var

def popColumns(columns: list, columnsToPop: list[int]|bool) -> None:
    if columnsToPop:
        for q in columnsToPop:
            columns.pop(q)

def csvThirdPopSwap(filepath: p.Path, d1: p.Path, d2: p.Path, columnsToPop: list[int]|bool, columnsToSwap: list[list]|bool, rank: int|bool = False) -> int:
    try:
        with open(filepath, "r") as file, open(d1, "w") as destination1, open(d2, "w") as destination2:

            file.readline() # Skip column names.

            for i, line in enumerate(file):

                # Change rank from bool -> int
                if rank: rank = i + 1

                # "Pop Swap" the line.
                new_line = linePopSwap(line, columnsToPop, columnsToSwap, rank)

                # Add to correct file.
                if i % 3 != 1:
                    destination1.write(new_line)
                else:
                    destination2.write(new_line)

        return 1
    except (FileNotFoundError, FileExistsError):
        print("There was an error when trying to open the file.")
        return 0

# split one csv into two as well as popping columns and swapping columns
def csvHalfPopSwap(filepath: p.Path, d1: p.Path, d2: p.Path, columnsToPop: list[int]|bool, columnsToSwap: list[list]|bool, rank: int|bool = False) -> int:
    if not filepath.exists():
        return 0
    
    try:
        with open(filepath, "r") as file, open(d1, "w") as destination1, open(d2, "w") as destination2:

            file.readline() # Skip column names.

            for i, line in enumerate(file):

                # Change rank from bool -> int
                if rank: rank = i + 1

                # "Pop Swap" the line.
                new_line = linePopSwap(line, columnsToPop, columnsToSwap, rank)

                # Add to correct file.
                if i % 2 == 0:
                    destination1.write(new_line)
                else:
                    destination2.write(new_line)

        return 1
    except (FileNotFoundError):
        print("Unable to open the file " + filepath)
        return 0

def csvPopSwap(filepath: p.Path, newFilepath: p.Path, columnsToPop: list[int]|bool, columnsToSwap: list[list]|bool, rank: int|bool = False) -> int:
    try:
        with open(filepath, "r+") as file, open(newFilepath, "w+") as destination:

            file.readline() # Skip column names.

            for i, line in enumerate(file):

                # Change rank from bool -> int
                if rank: rank = i + 1

                # "Pop Swap" the line where specified.
                new_line = linePopSwap(line, columnsToPop, columnsToSwap, rank)

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

    # 2020
    fp = p.Path(dir, "2020.csv")
    fp1 = p.Path(dir, "2020a.csv")
    fp2 = p.Path(dir, "2020b.csv")
    ctp = [1, 0]
    cts = False

    csvThirdPopSwap(fp, fp1, fp2, ctp, cts)

    fp = p.Path(dir, "2020a.csv")
    fp1 = p.Path(dir, "2020a_threshold.csv")
    vc = 0
    s = 4
    thresholdColumn(fp, fp1, vc, s)

    fp = p.Path(dir, "2020b.csv")
    fp1 = p.Path(dir, "2020b_threshold.csv")
    thresholdColumn(fp, fp1, vc, s)

    # 2019
    fp = p.Path(dir, "2019.csv")
    fp1 = p.Path(dir, "2019a.csv")
    fp2 = p.Path(dir, "2019b.csv")
    ctp = [1, 0]
    cts = False
    csvThirdPopSwap(fp, fp1, fp2, ctp, cts)

    fp = p.Path(dir, "2019a.csv")
    fp1 = p.Path(dir, "2019a_threshold.csv")
    thresholdColumn(fp, fp1, vc, s)

    fp = p.Path(dir, "2019b.csv")
    fp1 = p.Path(dir, "2019b_threshold.csv")
    thresholdColumn(fp, fp1, vc, s)

    # 2018
    fp = p.Path(dir, "2018.csv")
    fp1 = p.Path(dir, "2018a.csv")
    fp2 = p.Path(dir, "2018b.csv")
    ctp = [1, 0]
    cts = False
    csvThirdPopSwap(fp, fp1, fp2, ctp, cts)

    fp = p.Path(dir, "2018a.csv")
    fp1 = p.Path(dir, "2018a_threshold.csv")
    thresholdColumn(fp, fp1, vc, s)

    fp = p.Path(dir, "2018b.csv")
    fp1 = p.Path(dir, "2018b_threshold.csv")
    thresholdColumn(fp, fp1, vc, s)

    # 2017 (bad data - remove first column line 24 (number 71))
    # and then uncomment and run thresholdColumn (commented out below).
    # comment out the csvThirdPopSwap before running thresholdColumn and after
    # removing the 71, csvThirdPopSwap -> Remove bad data -> thresholdColumn(s)
    
    '''
    fp = p.Path(dir, "2017.csv")
    fp1 = p.Path(dir, "2017a.csv")
    fp2 = p.Path(dir, "2017b.csv")
    ctp = [1, 0]
    cts = False
    csvThirdPopSwap(fp, fp1, fp2, ctp, cts)
    '''
    
    fp = p.Path(dir, "2017a.csv")
    fp1 = p.Path(dir, "2017a_threshold.csv")
    thresholdColumn(fp, fp1, vc, s)

    fp = p.Path(dir, "2017b.csv")
    fp1 = p.Path(dir, "2017b_threshold.csv")
    thresholdColumn(fp, fp1, vc, s)

    # 2016
    fp = p.Path(dir, "2016.csv")
    fp1 = p.Path(dir, "2016a.csv")
    fp2 = p.Path(dir, "2016b.csv")
    ctp = [2, 1, 0]
    cts = False
    csvThirdPopSwap(fp, fp1, fp2, ctp, cts)

    fp = p.Path(dir, "2016a.csv")
    fp1 = p.Path(dir, "2016a_threshold.csv")
    thresholdColumn(fp, fp1, vc, s)

    fp = p.Path(dir, "2016b.csv")
    fp1 = p.Path(dir, "2016b_threshold.csv")
    thresholdColumn(fp, fp1, vc, s)

    # 2015
    fp = p.Path(dir, "2015.csv")
    fp1 = p.Path(dir, "2015a.csv")
    fp2 = p.Path(dir, "2015b.csv")
    ctp = [2, 1, 0]
    cts = False
    csvThirdPopSwap(fp, fp1, fp2, ctp, cts)

    fp = p.Path(dir, "2015a.csv")
    fp1 = p.Path(dir, "2015a_threshold.csv")
    thresholdColumn(fp, fp1, vc, s)

    fp = p.Path(dir, "2015b.csv")
    fp1 = p.Path(dir, "2015b_threshold.csv")
    thresholdColumn(fp, fp1, vc, s)

if __name__ == '__main__':
    main()