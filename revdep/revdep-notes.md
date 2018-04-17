Notes from revdepchecks 2018-04-17 for v1.1.0

First pass moderately successful. Here's how I closed it out.

### Java problems

Affected: BEACH, DataLoader, ie2misc, joinXL

Reinstalled Java and rJava.

https://stackoverflow.com/a/47685001/2825349
sudo ln -f -s $(/usr/libexec/java_home)/lib/server/libjvm.dylib /usr/local/lib

https://stackoverflow.com/a/44686316/2825349
sudo ln -s $(/usr/libexec/java_home)/jre/lib/server/libjvm.dylib /Library/Frameworks/R.framework/Libraries

R CMD javareconf, at various points, for good measure

While I was at it, I deleted this entry from `~/.Renviron` and so far my Java life has not gotten any worse:

```
JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk1.8.0_121.jdk/Contents/Home/jre
```

### Tcl/Tk problems

Affects: zooaRchGUI, dynBiplotGUI

It appears that revdepcheck hangs on packages that launch Tcl/Tk widgets during check. I have seen this hang ... then crash R. Or sometimes it hangs ... then records that check timed out for that package. I check these manually.

zooaRchGUI:
❯ checking CRAN incoming feasibility ... NOTE
  Maintainer: ‘Erik Otarola-Castillo <eoc@purdue.edu>’
  The Date field is over a month old.
0 errors ✔ | 0 warnings ✔ | 1 note ✖
R CMD check succeeded

dynBiplotGUI:
❯ checking CRAN incoming feasibility ... NOTE
  Maintainer: ‘Jaime Egido <jegido@usal.es>’
  The Date field is over a month old.
0 errors ✔ | 0 warnings ✔ | 1 note ✖
R CMD check succeeded

### JAGS

zooaRchGUI need rjags needs JAGS. And I did install JAGS but revdepcheck still struggles with zooaRchGUI for Tck/Tk reasons. Manual check.

### Fortran

GWSDAT depends on classInt, which just updated on CRAN. No Mac binary is available. But classInt also includes Fortran and something is screwed up right now with my ability to build fortran-containing R packages. So revdepcheck in unable to get the necessary dependencies installed and I can't figure out how to intervene and provide, say, the previous version of classInt. Manual check.

GWSDAT:
0 errors ✔ | 0 warnings ✔ | 0 notes ✔
R CMD check succeeded

RCzechia shows one failing example (due to lack of classInt) and also 1 test failure. All seems OK based on this manual check.

❯ checking examples ... NOTE
0 errors ✔ | 0 warnings ✔ | 1 note ✖
R CMD check succeeded

### GTK Hell

Affects: rattle, Ricetl

I'm giving up on getting all this installed: RGtk2, cairoDevice, gWidgetsRGtk2, playwith, rggobi, RGtk2Extras. I've never succeeded in the past.
