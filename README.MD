# copycat
Reverse engineer the classic <code>cat</code> Unix program to understand its functionality and implementation details

## Why?
Good question. In all honesty, I started with this small project as a way to warm up to a bigger project that I plan to do, and it is nice to use the knowledge I have learned in AP(COMS W3157) for something fun right? 

Besides, it was quite interesting to, without looking at the source code, deconstruct the properties and behavior of the program. In a way, I see this not only as a warming up exercise, but as a design problem. 

It is also through this project that I learned to appreciate the <code>man</code> page more. In fact, I was quite surprised by the amount of functionalities that <code>cat</code> has to offer. 

Fun easter egg: https://harmful.cat-v.org/cat-v/

## Observations of the program's behavior
Although of the flags were straightforward to implement, there were two flags that required a bit of extra research

<code>-l</code> If the output file is already locked, cat will block until the lock is acquired.

<code>-u</code> Disable output buffering

Disable output buffering was quite straightforward to implement, however, I think the more interesting one was <code>-l</code>. What it basically does is that it makes everyone wait for their turn. 

It is quite interesting to see how these flags plays into the idea that **Everything is a file** in UNIX

Other weird things that I noticed about <code>cat</code> is that when you call:
<code>cat -bn</code> or <code>cat -nb</code>

They all behave like <code>cat -b</code>

To be nitpicking, if you do <code>cat file1 -flag file2</code>, you will see that it treats the flag as a file. Or in other word, the program cat doesn't allow you to put your flag after a file, to yield I would say more fun behavior. 

Of course, these are only a few bizzare things that I saw. 

## Minor improvements to cat
With what I have said above, there are two features I have implemented:
* For <code>cat -bn</code> and <code>copycat -nb</code>, the last declared flag will be the one chosen
* You can do <code>cat file1 -flag file2</code> and this will happen:
```
>./copycat file1 -n file1

Monday
Tuesday
Wednesday
1   Monday
2   Tuesday
3   Wednesday
```

## Approach to design, probably bad practice
While fgets() or fread() are quite common ways to read a file or read the STREAM, because I have to deal with displaying non-print characters like ^C, ^? or ^I, I decided to use fgetc() to read the characters. This allows for me an easier time dealing with different flags input, and perhaps make the program visually flow better in my eyes.

To summarize, here is my code design tree, or hierachy of flags
```
                    _________main()________
                     /                  \   \     
        processFile()                   -l  -u
        /   |  \   \
        -s -n   -b  processChars()
                        /    \
                        -e    -v
                                \
                                -t
``` 
What is happening is that in main(), I call directly flags that explicitly deals with STDOUT, while for flags that deals more personally with formatting, I have it directly in processFile(), these are <code>-s</code> <code>-n</code>and <code>-b</code>, which are different flavor of line numberings and blank space dealing. 

To go deeper, while reading the manual page for <code>cat</code>, there seems to be a fixed format on how to deal with non-printing characters. As we are reading byte by byte, I thought that a good idea is that before we send each bytes to STDOUT, to run it through processChars() and check if the flags are activated and that conditions are met. 

In the end, we ended up with around 300 lines of code. 

*If there are any comments or suggestions, please let me know!*