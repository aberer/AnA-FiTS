#! /usr/bin/Rscript

args <- commandArgs(trailingOnly = TRUE)


if (length(args)  != 2) 
  print("Usage: ./script <af> <sfs-frag> ")

levs= factor(c("AnA-FiTS", "SFS_CODE-frag", "SFS_CODE-mono"))


af.for = read.table(args[1])
sfs = read.table(args[2])
## sfs.slow = read.table(args[3])

theMax = 0
theMax = max(theMax, af.for[,2])
theMax = max(theMax, sfs[,2])
## theMax = max(theMax, sfs.slow[,2])

pdf("sfs.pdf", width=6, height=6)
matplot(af.for[,1], af.for[,2],
        ## xlab="site frequency (in 50 samples)",
        xlab="",
        ylab="frequency", type="p", pch="o", col=1, lty=1, lwd=3, ylim=c(0,theMax))
matlines(sfs[,1], sfs[,2], col=2, lty=1, lwd=3, type="p", pch="x" )
## matlines(sfs.slow[,1], sfs.slow[,2], col=3, lty=1, lwd=3, type="p", pch="*" )
legend("topright", legend=levs, pch=c('o', 'x'), col=1:2)
bla = dev.off()

