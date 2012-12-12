#! /usr/bin/Rscript

## library(sm)

args <- commandArgs(trailingOnly = TRUE)

if (length(args)  != 2) 
  print("Usage: ./script  <af> <sfs-frag>  ")

levs= factor(c("AnA-FiTS", "SFS_CODE" ))


## read tables 
anafits = read.table(args[1])
anafits[,2] = levs[1]

sfs.frag = read.table(args[2])
sfs.frag[,2] = levs[2]

## sfs.mono = read.table(args[3])
## sfs.mono[,2] = levs[3]




data = rbind(anafits, sfs.frag)
## data = rbind(data, sfs.mono)

names(data) = c("values", "type")

theRange = range(data$values)
myBreaks = seq(theRange[1],theRange[2]) 

relevant = data[data$type == levs[1],1]
theSum = sum(relevant)
anafits = c()
for (elem in myBreaks )  
  anafits  = c(anafits, sum(relevant == elem) / theSum)

relevant = data[data$type == levs[2],1]
theSum = sum(relevant)
sfs.frag = c()
for (elem in myBreaks )  
  sfs.frag  = c(sfs.frag, sum(relevant == elem) / theSum)


## relevant = data[data$type == levs[3],1]
## theSum = sum(relevant)
## sfs.mono = c()
## for (elem in myBreaks )  
##   sfs.mono  = c(sfs.mono, sum(relevant == elem) / theSum)

df = data.frame(mids=myBreaks, anafits = anafits, sfs.frag = sfs.frag   )

pdf("numHap.pdf", width=5, height=5)
matplot(df$mids, df[,c(2,3)], type="l",lwd=3, lty=1,
        xlab="",
        ylab="density")
legend("topleft", legend=levs, fill=1:2)
bla = dev.off()

