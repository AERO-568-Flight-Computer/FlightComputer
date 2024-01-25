## Project uses gitflow stucture.  This means:
### main:
protected branch - only code that has been tested and reviewed gets merged into main.  And this only gets merges from the develop branch.
### develop:
somewhat protected branch - when you work on changes, you should make a new branch from develop to get started.  We call these "feature branches".  You want these to be very short-lived, so don't plan to pull a branch and work on it all semester.
### <feature branches>
This is where you work on your code.  When you have tested your individual code and it doesn't break everything, merge it back into develop.


The idea here is that even with a group of people making changes all at once: main is always well tested code, and develop is always a good starting point for your next code updates.
