import pathlib as p

def csvRemoveColumns(filepath: p.Path, newFilepath: p.Path, columns: list) -> int:
    try:
        with open(filepath, "r+") as f:

            f.readline() # skip columns

            lines = f.readlines()

            with open(newFilepath, "w+") as f1:
                for i in lines:
                    nl = i.split(',')
                    for q in columns:
                        nl.pop(q)
                    ns = ",".join(nl)
                    f1.write(ns)

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
    ffc = [2, 1, 0]

    csvRemoveColumns(fp, nfp, ffc)

    fp1 = p.Path("world_happiness_data", "2020.csv")
    nfp1 = p.Path("world_happiness_data", "2020rr15c.csv")
    ffc1 = [18, 17, 16, 15, 14, 13, 12, 5, 4, 1, 0]

    csvRemoveColumns(fp1, nfp1, ffc1)

if __name__ == '__main__':
    main()