library(reshape2)
library(ggplot2)
setwd("C:/Users/P285100/Desktop/IFD_activity/IFD_activity")

str1 <- "03_29-11activities"
data <- read.table(paste0(str1, ".txt"), sep="\t", header = F)

df <- (t(subset(data, select = -c(V1, V1002))))
colnames(df) <- data$V1

length(df[1,])

minwv = 0.0;     # minimal (weight) value
maxwv = 1.0;     # maximal (weight) value
steps = 101;  # num. of bins across range
stepsize = (maxwv - minwv)/steps  # bin range size


#Frequency matrix:

mtrxwP1 <- matrix(nrow = steps, ncol = length(df[1,]), dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
vecFpos <- vector()

for (i in 1:length(df[1,])){
  
  vecFpos[i] = length(which(df[,i] > 0.05))/length(df[,1])
  
  mtrxwP1[,i] = table(cut(df[,i], seq(minwv, maxwv, stepsize), right=T))/1000
  
}

colnames(mtrxwP1) <- data$V1


P2P <- ggplot(data = melt(t(mtrxwP1)), aes(x=Var1, y=Var2, fill=value)) + labs(x="generations", y="activity") + 
  geom_tile() + scale_fill_gradientn(colours = colorRampPalette(c("white", "red", "blue"))(3), 
                                     values = c(0, 0.05 , 0.3), space = "Lab", guide = FALSE) + geom_hline(yintercept = 0)+ theme_bw() +
  theme(axis.title.x=element_text(size=14), axis.title.y=element_text(size=12), panel.grid.major = element_blank(), panel.grid.minor = element_blank(),axis.line = element_line(colour = "black"), legend.position = "none")

P2P

ggsave(paste0(str1, ".png"), P2P)




#frequencies of morphs, custom
df_H = data.frame(data$V1, vecFpos, 1 - vecFpos)
colnames(df_H) <- c("gens", "highact", "lowact")

frequencies <- ggplot(data=df_H, aes(x=gens, y=highact, colour="weight > 0")) +
  geom_line(colour="darkgreen")  + theme(axis.title=element_text(size=12))+ 
  geom_line(data = df_H, aes(x = gens, y = lowact, colour="weight < 0"), colour="purple")  + 
  labs(x="", y="Herbivore frequency", colour="")+ theme(legend.position='none') +
  theme(panel.background = element_blank(), panel.border = element_rect(colour = "black", fill=NA),  
        axis.line = element_line(colour = "black"), axis.title.y = element_text(size = 9), axis.text=element_text(size=8))


#####
# IFD measures

idf_data <- read.table("IDF.txt", header = T)

plot(idf_data$G, idf_data$avg_ttifd)




