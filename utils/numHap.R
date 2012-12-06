#! /usr/bin/Rscript

## library(sm)

args <- commandArgs(trailingOnly = TRUE)

if (length(args)  != 5) 
  print("Usage: ./script  <numHapAF-for> <numHapAF-mix> <numHapAF-anc> <numHapMs> ")



if (args[1] == "isMS") {
  levs= factor(c("AF-FOR", "AF-MIX", "AF-ANC", "MS"))
} else {
  levs= factor(c("AF-FOR", "AF-MIX", "AF-ANC", "SFS_CODE"))
}


af.for = read.table(args[2])
af.for[,2] = levs[1]

af.mix = read.table(args[3])
af.mix[,2] = levs[2]

af.anc = read.table(args[4])
af.anc[,2] = levs[3]

ms = read.table(args[5])
ms[,2] = levs[4]

data = rbind(af.for, af.mix)
data = rbind(data, af.anc)
data = rbind(data, ms)

names(data) = c("values", "type")

theRange = range(data$values)
myBreaks = seq(theRange[1],theRange[2]) 


relevant = data[data$type == levs[1],1]
theSum = sum(relevant)
affor = c()
for (elem in myBreaks )  
  affor  = c(affor, sum(relevant == elem) / theSum)


relevant = data[data$type == levs[2],1]
theSum = sum(relevant)
afmix = c()
for (elem in myBreaks )  
  afmix  = c(afmix, sum(relevant == elem) / theSum)

relevant = data[data$type == levs[3],1]
theSum = sum(relevant)
afanc = c()
for (elem in myBreaks )  
  afanc  = c(afanc, sum(relevant == elem) / theSum)

relevant = data[data$type == levs[4],1]
theSum = sum(relevant)
ms = c()
for (elem in myBreaks )  
  ms  = c(ms, sum(relevant == elem) / theSum) 

df = data.frame(mids=myBreaks, affor = affor,  afmix = afmix,  afanc = afanc,  ms = ms  )

pdf("numHap.pdf", width=5, height=5)
matplot(df$mids, df[,c(2,3,4,5)], type="l",lwd=3, lty=1,
        ## xlab="number of haplotypes",
        xlab="",
        ylab="density")
legend("topleft", legend=levs, fill=1:4)
## print("number of haplotypes")
## print(table(data))
## matplot(table(data), type="l",lwd=3, lty=1,  xlab="number of distinct haplotypes", ylab="frequency")
## legend("topleft", legend=levs, fill=1:4)
bla = dev.off()

