import pathlib as p
import math as m

def swapColumns(columns, swap1, swap2):
    var = columns[swap1]
    columns[swap1] = columns[swap2]
    columns[swap2] = var

def popColumns(columns, columnsToPop):
    for q in columnsToPop:
        columns.pop(q)

def csvRemoveColumns(filepath: p.Path, newFilepath: p.Path, columnsToPop: list, swapThoseColumns: bool) -> int:
    try:
        with open(filepath, "r+") as f:

            columns2 = f.readline().split(',') # skip columns
            popColumns(columns2, columnsToPop)
            swapColumns(columns2, 5, 6)

            lines = f.readlines()

            with open(newFilepath, "w+") as f1:
                for i in lines:
                    nl = i.split(',')
                    popColumns(nl, columnsToPop)
                    if swapThoseColumns: swapColumns(nl, 5, 6) # swap columns could be called better, not literally;
                    ns = ",".join(nl)
                    f1.write(ns + "\n")

        return 1

    except (FileNotFoundError, FileExistsError):
        print("There was an error when trying to open a file.")
        return 0


def main():

    # 2015
    # Country,Region,Happiness Rank,Happiness Score,Standard Error,Economy (GDP per Capita),Family,Health (Life Expectancy),Freedom,Trust (Government Corruption),Generosity,Dystopia Residual
    # 12 columns

    # 2020
    # Country name,Regional indicator,Ladder score,Standard error of ladder score,upperwhisker,lowerwhisker,Logged GDP per capita,Social support,Healthy life expectancy,Freedom to make life choices,Generosity,Perceptions of corruption,Ladder score in Dystopia,Explained by: Log GDP per capita,Explained by: Social support,Explained by: Healthy life expectancy,Explained by: Freedom to make life choices,Explained by: Generosity,Explained by: Perceptions of corruption,Dystopia + residual
    # 20 columns

    fp = p.Path("world_happiness_data", "2015.csv")
    nfp = p.Path("world_happiness_data", "2015rr20c.csv")
    ffc = [11, 4, 2, 1, 0]

    csvRemoveColumns(fp, nfp, ffc, True)

    fp1 = p.Path("world_happiness_data", "2020.csv")
    nfp1 = p.Path("world_happiness_data", "2020rr15c.csv")
    ffc1 = [19, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 1, 0]

    csvRemoveColumns(fp1, nfp1, ffc1, False)

if __name__ == '__main__':
    main()