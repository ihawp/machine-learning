import pathlib as p

# Everything in here is garbage. year17 has the most mind bending bug even with completely different code!?!?! maybe its just my system though *fingers crossed* :()
# The goal of this script is to split the csv files into two csv files, where the odd lines are in one file, and even lines in the other.
# Split the data right down the middle into training and test data..is that a good ML tactic?

def year1819(filename, outputFolder = "world_happiness_data"):
    with open(p.Path(outputFolder, filename + ".csv"), "r", -1) as f:

        f.readline() # leave this to skip line

        rl = f.readlines()

        # only remove the second index in row + the top columns

        with open(p.Path(outputFolder, filename + "a.csv"), "w+", -1) as f1:
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

                    print(new_string)

                    f1.write(new_string)
                    rl.pop(i)
                else:
                    break
            f1.close()

        with open(p.Path(outputFolder, filename + "b.csv"), "w+", -1) as f2:
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

        with open(p.Path(outputFolder, filename + "a.csv"), "w+", -1) as f1:
            for i in range(len(rl)):
                if i < len(rl):
                    string = rl[i]
                    split_string = string.split(',')
                    
                    print("FIRST Split string", split_string[0], split_string)
                    split_string.pop(0)

                    print("Split string", split_string, "\n")

                    strim_enum = ",".join(split_string)
                    print(strim_enum, "\n")
                    f1.write(strim_enum)
                    rl.pop(i)
                else:
                    break
            f1.close()

        with open(p.Path(outputFolder, filename + "b.csv"), "w+", -1) as f2:
            for i in range(len(rl)):
                if i < len(rl):
                    string = rl[i]
                    split_string = string.split(',')
                    
                    print("FIRST Split string", split_string)
                    split_string.pop(0)

                    print("Split string", split_string, "\n")

                    strim_enum = ",".join(split_string)
                    f2.write(strim_enum)
                else:
                    break
            f2.close()

        f.close()

def year1516(filename, outputFolder = "world_happiness_data"):

    with open(p.Path(outputFolder, filename + ".csv"), "r", -1) as f:

        columns = f.readline() # leave this to skip line

        rl = f.readlines()

        with open(p.Path(outputFolder, filename + "a.csv"), "w+", -1) as f1:
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

        with open(p.Path(outputFolder, filename + "b.csv"), "w+", -1) as f2:
            for i in range(len(rl)):
                loc = rl[i].find(",")
                ns = rl[i][loc + 1 : len(rl[i])]
                loc2 = ns.find(",")
                ns2 = ns[loc2 + 1 : len(ns)]
                f2.write(ns2)
            f2.close()

        f.close()

def year20(filename, outputFolder = "world_happiness_data"):

    with open(p.Path(outputFolder, filename + ".csv"), "r", -1) as f:

        columns = f.readline() # leave this to skip line

        rl = f.readlines()

        with open(p.Path(outputFolder, filename + "a.csv"), "w+", -1) as f1:
            for i in range(len(rl)):
                if i < len(rl):
                    loc = rl[i].find(",")
                    ns = rl[i][loc + 1 : len(rl[i])]
                    loc2 = ns.find(",")
                    ns2 = str(i + 1) + "," + ns[loc2 + 1 : len(ns)]
                    f1.write(ns2)
                    rl.pop(i)
                else:
                    break
            f1.close()

        with open(p.Path(outputFolder, filename + "b.csv"), "w+", -1) as f2:
            for i in range(len(rl)):
                loc = rl[i].find(",")
                ns = rl[i][loc + 1 : len(rl[i])]
                loc2 = ns.find(",")
                ns2 = str(i + 1) + "," + ns[loc2 + 1 : len(ns)]
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
    ]

    first_second_column_text_no_rank = [
        "2020"
    ]

    second_column_text = [
        "2018",
        "2019"
    ]

    for i in range(len(first_second_column_text_no_rank)):
        year20(first_second_column_text_no_rank[i])

# a files take the odd numbers
# b files take the even numbers

if __name__ == '__main__':
    main()