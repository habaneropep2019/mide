# Fork

There are approximately 806,000 lines of code in this project altogether. 616,000 lines of code are C source, C++ source, or header files.

That doesn't mean all of those lines of code have to be attended to individually, however I think it gives a good perspective on the size of this project.

I suspect a fair amount of code will have to be re-written to port to a newer version of Qt, simply due to how it has changed since Qt1. That's one of the largest tasks in this project.

The second major task is re-branding from KDE to StuDE.

The third major task is updating components to work on the newer X11 protocols.

Until many components are updated, they will simply just have to be left out until they are inegrated back in again, as we don't want broken pieces.

The first components that have to be worked on will be kdebase and kdelibs. Next will be either kedadmin or kdeutils, or possibly both.