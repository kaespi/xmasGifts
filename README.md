# xmasGifts
Commandline tool to organize the assignments donors-giftee in your family

## Background
In our family we decided to limit the number of Xmas gifts to one per person. Therefore each family member was essentially once donor and once giftee, i.e. each family member got another member assigned to offer him a present. Over the years we wanted to optimize it slightly to make things cooler. So, the donor/giftee assignment was asked to fulfill the following requirements:

* every person is once donor
* every person is once giftee
* the list of person 1 -> person 2 -> person 3 ... (with person 2 being giftee for donor person 1, person 3 being giftee for donor person 2, ...) should form one circle
* every donor may have a list of people which are excluded as his giftees (e.g. because they were already his giftees in former years)

## Build Instructions

Compile the software with `make all`.

## Usage

Run the tool in the command line with

```
xmasGifts [-v] <config file>
```

with `<config file>` being a configuration. Additionally a `-v` increases verbosity level. The format of the configuration file is explained in more details in the next section

### Configuration File

The configuration file contains one line per participant. The first entry on the line is the name of the participant. Note that no whitespace (e.g. tab, space) is allowed in names. The tool will behave incorrect in this case. After the participant's name ("donor") follows a list (comma, semicolon, or whitespace separated) of other particpants which have to be excluded as the giftees. Consider the exemplary configuration file:

```
 Alice Bob
 Bob   Peter,Tom
 Tom   Alice
 Peter Bob
```

This file means that Bob is excluded as giftee for Alice. Bob cannot be donor for Peter and Tom, etc. A valid solution in this configuration would be e.g. Alice -> Peter -> Tom -> Bob -> Alice, i.e. Alice has Peter assigned as her giftee, Peter is donor for Tom, Tom is donor for Bob, etc.

### Output File

The tool will write two output files, postfixed with `_cards.txt` and `_envelopes.txt`. They contain the randomized mapping numbers to participants (`_cards.txt`) and the instructions which card number to fill into which envelope number (`_envelopes.txt`).

For the above example we would have e.g. `..._cards.txt`:
```
 0 - Alice
 1 - Tom
 2 - Peter
 3 - Bob
```

and `..._envelopes.txt`:
```
Card 2 into envelope 0
Card 1 into envelope 2
Card 3 into envelope 1
Card 0 into envelope 3
```

If you let two different people look at the files and prepare the material, then no one knows anything, supposedly.
