import pathlib as p

def year1819(filename, outputFolder = "world_happiness_data"):
    with open(p.Path(outputFolder, filename + ".csv"), "r", -1) as f:

        f.readline() # leave this to skip line

        rl = f.readlines()

        # only remove the second index in row + the top columns

        with open(p.Path(outputFolder + "/" + filename + "a.csv"), "w+", -1) as f1:
            for i in range(len(rl)):
                # find index of first and second commas,
                # remove the first comma and everything 
                # after until before the second comma
                if i < len(rl):
                    this_string = rl[i]
                    first_comma = this_string.find(",")
                    second_comma = this_string.find(",", first_comma + 1)

                    first_half_string = this_string[0 : first_comma]
                    second_half_string = this_string[second_comma : len(this_string)]

                    new_string = first_half_string + second_half_string

                    f1.write(new_string)
                    rl.pop(i)
                else:
                    break
            f1.close()

        with open(p.Path(outputFolder + "/" + filename + "b.csv"), "w+", -1) as f2:
            for i in range(len(rl)):
                if i < len(rl):
                    this_string = rl[i]
                    first_comma = this_string.find(",")
                    second_comma = this_string.find(",", first_comma + 1)

                    first_half_string = this_string[0 : first_comma]
                    second_half_string = this_string[second_comma : len(this_string)]

                    new_string = first_half_string + second_half_string

                    f2.write(new_string)
                else:
                    break
            f2.close()

        f.close()

def year17(filename, outputFolder = "world_happiness_data"):
    with open(p.Path(outputFolder, filename + ".csv"), "r", -1) as f:

        f.readline() # leave this to skip line

        rl = f.readlines()

        with open(p.Path(outputFolder + "/" + filename + "a.csv"), "w+", -1) as f1:
            for i in range(len(rl)):
                if i < len(rl):
                    loc = rl[i].find(",")
                    ns = rl[i][loc + 1 : len(rl[i])]
                    f1.write(ns)
                    rl.pop(i)
                else:
                    break
            f1.close()

        with open(p.Path(outputFolder + "/" + filename + "b.csv"), "w+", -1) as f2:
            for i in range(len(rl)):
                loc = rl[i].find(",")
                ns = rl[i][loc + 1 : len(rl[i])]
                f2.write(ns)
            f2.close()

        f.close()

def year151620(filename, outputFolder = "world_happiness_data"):

    with open(p.Path(outputFolder, filename + ".csv"), "r", -1) as f:

        columns = f.readline() # leave this to skip line

        rl = f.readlines()

        with open(p.Path(outputFolder + "/" + filename + "a.csv"), "w+", -1) as f1:
            for i in range(len(rl)):
                if i < len(rl):
                    loc = rl[i].find(",")
                    ns = rl[i][loc + 1 : len(rl[i])]
                    loc2 = ns.find(",")
                    ns2 = ns[loc2 + 1 : len(ns)]
                    f1.write(ns2)
                    rl.pop(i)
                else:
                    break
            f1.close()

        with open(p.Path(outputFolder + "/" + filename + "b.csv"), "w+", -1) as f2:
            for i in range(len(rl)):
                loc = rl[i].find(",")
                ns = rl[i][loc + 1 : len(rl[i])]
                loc2 = ns.find(",")
                ns2 = ns[loc2 + 1 : len(ns)]
                f2.write(ns2)
            f2.close()

        f.close()

def main():

    first_column_text = [
        "2017"
    ]

    first_second_column_text = [
        "2015",
        "2016",
        "2020"
    ]

    second_column_text = [
        "2018",
        "2019"
    ]

    for i in range(len(second_column_text)):
        year1819(second_column_text[i])

if __name__ == '__main__':
    main()