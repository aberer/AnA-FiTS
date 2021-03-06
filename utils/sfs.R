#! /usr/bin/Rscript

args <- commandArgs(trailingOnly = TRUE)


if (length(args)  != 5) 
  print("Usage: ./script <sfsAF-for> <sfsAF-mix> <sfsAF-anc> <sfsMs> ")


if (args[1] == "isMS")  {
  levs= factor(c("AF-FOR", "AF-MIX", "AF-ANC", "MS"))
} else  {
  levs= factor(c("AF-FOR", "AF-MIX", "AF-ANC", "SFS_CODE"))
}

af.for = read.table(args[2])
af.mix = read.table(args[3])
af.anc = read.table(args[4])
ms = read.table(args[5])

theMax = 0
theMax = max(theMax, af.for[,2])
theMax = max(theMax, af.mix[,2])
theMax = max(theMax, af.anc[,2])
theMax = max(theMax, ms[,2])


pdf("sfs.pdf", width=5, height=5)
matplot(af.for[,1], af.for[,2],
        xlab="",
        ## xlab="site frequency (in 10 samples)",
        ylab="frequency", type="p", pch="o", col=1, lty=1, lwd=3, ylim=c(0,theMax))
matlines(af.mix[,1], af.mix[,2], col=2, lty=1, lwd=3, type="p", pch="x" )
matlines(af.anc[,1], af.anc[,2], col=3, lty=1, lwd=3, type="p", pch="+" )
matlines(ms[,1], ms[,2], col=4, lty=1, lwd=3, type="p", pch='*' )
legend("topright", legend=levs, pch=c('o', 'x', '+', '*'), col=1:4)
bla = dev.off()

