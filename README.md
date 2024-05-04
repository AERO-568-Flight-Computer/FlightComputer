## Project uses gitflow stucture.  This means:
### main:
protected branch - only code that has been tested and reviewed gets merged into main.  And this only gets merges from the develop branch.
### develop:
somewhat protected branch - when you work on changes, you should make a new branch from develop to get started.  We call those "feature branches". Make sure you keep develop in a state where new work can pull from that and have a good starting point.
### "feature" branches
with a name like: feature/read_temperature_sensor

This is where you work on your code.  When you have tested your individual code and it doesn't break everything, merge it back into develop. You want these to be very short-lived, so don't plan to pull a branch and work on it all semester; get a small update working, then get it merged back into develop, then pull a new feature/branch for the next small step.


The idea here is that even with a group of people making changes all at once: main is always well tested code, and develop is always a good starting point for your next code updates.
