#! /usr/bin/Rscript


args <- commandArgs(trailingOnly = TRUE)

if (length(args)  != 4) 
  print("Usage: ./script <segSitesAF-for> <segSitesAF-mix> <segSitesAF-anc> <segSitesMs> ")


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

histObj = hist(data$values, plot=F)
myBreaks = histObj$breaks

affor.freq  = hist(data[ data$type == levs[1],1], breaks=myBreaks, plot=F)
affor.freq = affor.freq$counts / sum(affor.freq$counts )

afmix.freq  = hist(data[ data$type == levs[2],1], breaks=myBreaks, plot=F)
afmix.freq = afmix.freq$counts / sum(afmix.freq$counts )

afanc.freq  = hist(data[ data$type == levs[3],1], breaks=myBreaks, plot=F)
afanc.freq = afanc.freq$counts / sum(afanc.freq$counts )

ms.freq  = hist(data[ data$type == levs[4],1], breaks=myBreaks, plot=F)
ms.freq = ms.freq$counts / sum(ms.freq$counts )

df = data.frame(mids=histObj$mids, affor = affor.freq,  afmix = afmix.freq,  afanc = afanc.freq,  ms = ms.freq  )


pdf("segSites.pdf", width=6, height=6)
matplot(df$mids, df[,c(2,3,4,5)], type="l",lwd=3, lty=1,  xlab="number of segregating sites", ylab="density")
legend("topright", legend=levs, fill=1:4)
bla = dev.off()

