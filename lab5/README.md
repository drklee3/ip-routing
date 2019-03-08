# Lab 5 - Routing

## Layout

* `main.c` - main file yeah
  * `costs`
    * costs table
    * parses / calculates costs
  * `logger`
    * logging functions (info / debug / error)
  * `machine`
    * machine configurations
    * parses machines
  * `receiver`
    * listens for messages on thread #1
    * sends message to `costs` to update costs table
  * `sender`
    * reads user input for cost changes on main thread
    * sends cost update message to all other machines
  * `updater`
    * gets least cost array from `costs` every 10-20 seconds
