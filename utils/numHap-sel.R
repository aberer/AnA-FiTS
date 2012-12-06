#! /usr/bin/Rscript

## library(sm)

args <- commandArgs(trailingOnly = TRUE)

if (length(args)  != 2) 
  print("Usage: ./script <numHapAF-for> <numHapSFS> ")


levs = factor(c("AnA-FiTS", "SFS_CODE"))


af.for = read.table(args[1])
af.for[,2] = levs[1]

sfs = read.table(args[2])
sfs[,2] = levs[2]

data = rbind(af.for, sfs)
names(data) = c("values", "type")

histObj = hist(data$values, plot=F)
myBreaks = histObj$breaks

affor.freq  = hist(data[ data$type == levs[1],1], breaks=myBreaks, plot=F)
affor.freq = affor.freq$counts / sum(affor.freq$counts )

sfs.freq  = hist(data[ data$type == levs[2],1], breaks=myBreaks, plot=F)
sfs.freq = sfs.freq$counts / sum(sfs.freq$counts )

df = data.frame(mids=histObj$mids, affor = affor.freq, sfs = sfs.freq    )

pdf("numHap.pdf", width=6, height=6)
matplot(df$mids, df[,c(2,3)], type="l",lwd=3, lty=1,  xlab="number of haplotypes", ylab="density")
legend("topleft", legend=levs, fill=1:2)
## legend("topright", legend=levs, fill=1:2)
bla = dev.off()

