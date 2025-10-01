# A charcuterie of little projects!
## Python Webscraper
The oldest of these works. I had an opportunity that lent itself to trying my hand at automating web requests to generate some API keys. It is a rudimentary design, but for what was needed it worked and is well beyond EOL

## Parallel Programs
A sampling of lessons and interestings samples as I learned how to use both parallel and distributed computing techniques. I enjoyed thinking through some of the OpenMPI solutions especially for the Matrix solution. 
Largely, I wanted to learn how to design parallel programs. Additionally, I followed an old interest of mine and worked some math cases in for the fun of it. In my Pi Monte Carlo estimation, I learned the importance of 
using `rand_r()` for thread safe generations. Without it, `rand()` kept inhibiting the OpenMP solution to the point where it was often slower than sequential operation. Remember to use threadsafe functions!o

## Software Exploitation
A few things exist here. First, a nice reading list for those looking to get some more information on technqiues. Various blogs and other writeups that have helped me understand topics I have put there. 
Second, a challenge I wrote that utilizes uninitialized variables to win. This comes from a deep realization in my journey that memory is not actually cleaned when functions return or when memory is freed (to some degree)
Lastly, a setup script that I have been working on to try and load a system with tools I often use when working through these problems. It *mostly* works, tested on Ubuntu 20.04+
