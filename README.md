# xmasGifts

Commandline tool to organize the assignments donors-giftee in your family

## Background

In our family we decided to limit the number of Xmas gifts to one per person. Therefore each family member was essentially once donor and once giftee, i.e. each family member got another member assigned to offer him a present. Over the years we wanted to optimize it slightly to make things cooler. So, the donor/giftee assignment was asked to fulfill the following requirements:

* every person is once donor
* every person is once giftee
* the list of person 1 -> person 2 -> person 3 ... (with person 2 being giftee for donor person 1, person 3 being giftee for donor person 2, ...) should form one circle
* every donor may have a list of people which are excluded as his giftees (e.g. because they were already his giftees in former years)

## Build Instructions

Requires the two libraries

* libuuid (e.g. Debian/Ubuntu package `uuid-dev`)
* libquickmail (e.g. from [github](https://github.com/cdevelop/libquickmail))

Compile the software with `make all`.

## Usage

Run the tool in the command line with

```bash
xmasGifts [-v] [-r] [-e] [-u <username>] [-p <pwd>] [-f <sender>] [-s <smtpserver>] <config file>
```

with `<config file>` being a configuration. Additionally a `-v` increases verbosity level. The format of the configuration file is explained in more details in the next section.

The tool has two ways implemented to construct the "gift list":

* by default it's using a recursive (and systematic) approach which is guaranteed to either find the solution or conclude that it's not possible to construct a valid list with the given constraints
* if `-r` is used as command line parameter, it's using a purely random approach. It just randomly shuffles the participants. If the obtained list is valid it ends, otherwise it repeats that until a valid solution is obtained

The second approach, i.e. using `-r` is a reasonable choice if there exist not too many constraints, i.e. when it's likely to find a valid list with just a few random guesses. In all other cases the default option is preferable.

The option `-e` enables the parsing of email addresses as the 2nd column in the input file (see also "Configuration File with Email Addresses"). In this case emails will be sent to the participants disclosing to them who their respecitve giftee is. We found this to be quite cool as it reduces the logistic effort and broadcasts the information immediately. See "Sending Emails" below for more details on that.

### Basic Configuration File

The configuration file contains one line per participant. The first entry on the line is the name of the participant. Note that no whitespace (e.g. tab, space) is allowed in names. The tool will behave incorrect in this case. After the participant's name ("donor") follows a list (comma, semicolon, or whitespace separated) of other particpants which have to be excluded as the giftees. Consider the exemplary configuration file:

```text
 Alice Bob
 Bob   Peter,Tom
 Tom   Alice
 Peter Bob
```

This file means that Bob is excluded as giftee for Alice. Bob cannot be donor for Peter and Tom, etc. A valid solution in this configuration would be e.g. Alice -> Peter -> Tom -> Bob -> Alice, i.e. Alice has Peter assigned as her giftee, Peter is donor for Tom, Tom is donor for Bob, etc.

### Configuration File with Email Adresses

When using the email command line option `-e` the program expects people's email address in the 2nd column of the input file:

```text
 Alice  alice@aol.com      Bob
 Bob    bob@gmail.com      Peter,Tom
 Tom    tom@gmx.net        Alice
 Peter  peter@hotmail.com  Bob
```

The meaning of the other parts of the file don't change.

### Output File

The tool will write two output files, postfixed with `_cards.txt` and `_envelopes.txt`. They contain the randomized mapping numbers to participants (`_cards.txt`) and the instructions which card number to fill into which envelope number (`_envelopes.txt`).

For the above example we would have e.g. `..._cards.txt`:

```text
 0 - Alice
 1 - Tom
 2 - Peter
 3 - Bob
```

and `..._envelopes.txt`:

```text
Card 2 into envelope 0
Card 1 into envelope 2
Card 3 into envelope 1
Card 0 into envelope 3
```

If you let two different people look at the files and prepare the material, then no one knows anything, supposedly.

### Sending Emails

Emails will be sent using the Simple Mail Transfer Protocol (SMTP). In order to be able to do so a few information need to be provided on the command line, such as

* the SMTP server's address (`-s <smtpserver>`), port 25 will be used with basic authentication (username/password)
* the username and password to login to the SMTP server (`-u <username>` and `-p <pwd>`)
* the sender email address for all the emails (`-f <sender>`)

So e.g. like

```bash
xmasGifts -e -s smtp.abc.com -u fred@abc.com -p fredpassword123 -f fred@abc.com cfg.txt
```

The subject and body of the emails are hardcoded in the program, in the [`email.cpp`](src/email.cpp) file. So in case you don't want our German text in there, just edit the file and recompile.

There's no big magic in the email sending library. Therefore, some email providers might detect the emails as junk. So, probably you should warn the participants about an incoming email. At least some adaptations were made to let the message pass the Googlemail filter.
