# WeatherStation

A weather station project based on Arduino. Some more info and background in my [blog post about the project][BLOG].

## Compontents

The current parts of the setup consists of a humidity monitor and a server to monitor, display and log values

### humidity1

The Arduino script that currently takes the measurements, in my case with a [CM-R resistive humidity detector][HUM].

### Server

A Node.js server that takes the data and displays in your browser on a [graph][GRAPH]. Also longs into MongoDB, with parameters adjusted through the `MONGO_URL` and `MONGO_DB` environmental variables.

## License

[MIT license][MIT]: do whatever you like. See more info in `License`.

 [BLOG]: http://gergely.imreh.net/blog/2012/06/lets-talk-about-humidity/ "Let's talk about humidity, on ClickedyClick"
 [HUM]: http://file.yizimg.com/3381/20061221103057890280486.pdf "English spec sheet"
 [GRAPH]: http://file.yizimg.com/3381/20061221103057890280486.pdf "Example plot of humidity data"
 [MIT]: http://en.wikipedia.org/wiki/MIT_License "MIT License on Wikipedia"