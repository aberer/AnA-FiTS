#! /usr/bin/Rscript

library(sm)

args <- commandArgs(trailingOnly = TRUE)

if (length(args)  != 4) 
  print("Usage: ./script <numHapAF-for> <numHapAF-mix> <numHapAF-anc> <numHapMs> ")


levs= factor(c("AF-FOR", "AF-MIX", "AF-ANC", "MS"))

af.for = read.table(args[1])
af.for[,2] = levs[1]

af.mix = read.table(args[2])
af.mix[,2] = levs[2]

af.anc = read.table(args[3])
af.anc[,2] = levs[3]

ms = read.table(args[4])
ms[,2] = levs[4]

data = rbind(af.for, af.mix)
data = rbind(data, af.anc)
data = rbind(data, ms)

names(data) = c("values", "type")

pdf("numHap.pdf", width=6, height=6)
sm.density.compare(data$values,  data$type, xlab="number of haplotypes")
legend("topright", legend=levs, fill=2+(0:nlevels(data$type)))
bla = dev.off()

