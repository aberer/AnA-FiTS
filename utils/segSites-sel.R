#! /usr/bin/Rscript


args <- commandArgs(trailingOnly = TRUE)

if (length(args)  != 2) 
  print("Usage: ./script <af> <sfs-frag> ")


levs = factor(c("AnA-FiTS", "SFS_CODE"))


af.for = read.table(args[1])
af.for[,2] = levs[1]

sfs.frag = read.table(args[2])
sfs.frag[,2] = levs[2]

## sfs.mono = read.table(args[3])
## sfs.mono[,2] = levs[3]

data = rbind(af.for, sfs.frag)
## data = rbind(data, sfs.mono)
names(data) = c("values", "type")

histObj = hist(data$values, plot=F, breaks="FD")
myBreaks = histObj$breaks

affor.freq  = hist(data[ data$type == levs[1],1], breaks=myBreaks, plot=F)
affor.freq = affor.freq$counts / sum(affor.freq$counts )

sfs.freq  = hist(data[ data$type == levs[2],1], breaks=myBreaks, plot=F)
sfs.freq = sfs.freq$counts / sum(sfs.freq$counts )

## sfsmono  = hist(data[ data$type == levs[3],1], breaks=myBreaks, plot=F)
## sfsmono = sfsmono$counts / sum(sfsmono$counts )

df = data.frame(mids=histObj$mids, affor = affor.freq, sfs = sfs.freq )

pdf("segSites.pdf", width=6, height=6)
matplot(df$mids, df[,c(2,3)], type="l",lwd=3, lty=1,
        ## xlab="number of segregating sites",
        xlab= "" ,
        ylab="density")
legend("topright", legend=levs, fill=1:2)
bla = dev.off()

