# Lab 3: Project #3

## K-means: A Real Application Parallel Challenge

### 100 Points


### Caveat

Most of our CS 475/575 projects employ parallelism because I want you to see something dramatic or obvious. This one is really here to give you some experience with a real-ish application. Yes, there will be performance enhancements, but they will not necessarily be as in-your-face as some of the others. This is typical of many applications for parallel programming.

### Introduction

In an effort to unify the country, politicians have decided to throw away existing state boundaries and group cities in the same relative region into NUMCAPITALS new states, each with a new capital city at the centroid of the state. (Yes, I know this sounds wacky, but, honestly, does it sound any wackier than other ideas you've heard of?)

Fortunately, there is a well-known computer science algorithm called K-Means that handles this sort of thing.

### What is the K-Means Algorithm?

1. From the list of cities, you could choose NUMCAPITALS at random to be the initial state capitals. BUT DON'T DO IT THIS WAY! In the sample code, I picked them at uniform intervals from the list. We will use that instead. Start with these cities' longitude and latitude as the list of capitals.
2. Go through all the cities and figure out which of the NUMCAPITALS they are closest to. Use Pythagoran distance (square root of the sum of the squares of the Δlongitude and Δlatitude). Assign that city to belong to that capital. (Yes, I know using Pythagorean distance with longitude-latitude doesn't really give correct distance. But, it's clearer to understand. Humor me.)
3. Take all the cities in each group and average their longitudes and latitudes. That becomes the new set of capitals.
4. Go back to step #2.

Repeat this process until each city's state membership no longer changes. To make our version more straightforward, we will just repeat it 100 times.

### The Data

Thanks to the US Census Bureau, we have data on the 331 largest cities in the US. (Originally, there were 333, but I threw out the one in Alaska and the one in Hawaii as they would skew the average longitude-latitude calculations too much.)

Get the data file here: `UsCities.data`. Place it in the same folder as your `.cpp` file.

This will be included in your program with a `#include`, which is already in the sample code.

### Project Requirements

1. Start with this sample code, `proj03.cpp`. Place it in the same folder as your `UsCities.data` file.
2. Fill in the missing parts of the program, especially the `#pragma` that goes before the `NUMCITIES` for-loop.
3. The Makefile I used looks like this:

```make
proj03:		proj03.cpp
		g++  proj03.cpp    -o  proj03    -lm  -fopenmp
```

4. Run the program for some combination of `NUMT` and `NUMCAPITALS` values.
5. Vary the number of threads (`NUMT`) and the number of capital cities (`NUMCAPITALS`). I ended up using a script that looked like this:

```bash
#!/bin/bash
for t in 1 2 4 6 8
do
  for n in 2 3 4 5 10 15 20 30 40 50
  do
     g++   proj03.cpp  -DNUMT=$t -DNUMCAPITALS=$n  -o proj03  -lm  -fopenmp
    ./proj03
  done
done
```

This is what I did, but, you can handle the requirements any way you please.

### For Those New to C/C++ -- What are those struct Things?

In the code, you will see constructs like:

```c
struct city
{
        std::string     name;
        float           longitude;
        float           latitude;
        int             capitalnumber;
        float           mindistance;
};
```

Don't let that spook you. A struct is just a way to bundle a bunch of variables together under one name, `city` in this case. You get at each sub-element with a `.` (a period), like this:

```c
struct city    corvallis;
corvallis.name =      "Corvallis, OR";
corvallis.longitude = 123.27;
corvallis.latitude =   44.57;
```

An array of structs works just like an array of any other variable type. The cities are all in an array called `struct city Cities[ ]` (which is in the `UsCities.data` file). To, perhaps, do something with the longitude of city `#100`, we would write:

```c
... Cities[100].longitude ...
```

Or, if processing the cities in a for-loop:

```c
... Cities[i].longitude ...
```

### Commentary

Your commentary write-up (turned in as a PDF file) should include:

1. Tell what machine you ran this on
2. Tell what operating system you were using
3. Tell what compiler you used
4. Include the rectangular table of performance data.
5. Include a graph of performance vs. `NUMT` with the colored curves being `NUMCAPITALS`.
6. Include a graph of performance vs. `NUMCAPITALS` cities with the colored curves being `NUMT`.
7. Tell us what you discovered by doing this. What patterns are you seeing in the graphs?

### Where Are The States Located?

I recognize that this assignment is quite USA-centric, but that was the data I could readily find.
This will show you where the states are and what range of longitudes and latitudes cover the country.

### +5 Points Extra Credit

The politicians have decided not to create any brand-new capital cities from the ground-up, but to re-use existing ones. (Finally a sensible idea...) When you are done computing the final longitudes-latitudes of the `NUMCAPITALS`, go through the list of cities and print the name of the city that is closest to each capital's longitude-latitude. This will serve to confirm that your program really did spread the capital cities out across the country.

The name of each city is stored in a C++ string. To print the city name, you would print the variable `Capitals[k].name.c_str()` using a `%s` printf format.

### Grading

| Feature | Points |
| --- | ---: |
| Data Table | 25 |
| Graph of performance vs. number of threads with the colored curves being `NUMCAPITALS` | 25 |
| Graph of performance vs. number of capital cities with the colored curves being `NUMT` | 25 |
| Commentary (see topics above) | 25 |
| Extra Credit: print the list of the final capital cities | 5 |
| Potential Total | 105 |
