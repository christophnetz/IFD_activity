curve( dnorm( x , 0.67 , 0.16 ) * dnorm(x, 0, 0.1) * dnorm(x, 0.5, 0.2) , lty=2 )
curve(dgamma(x, 1, 1) + dgamma(x, 2, 1), xlim = c(0, 5))
integrate(dnorm, 0, 0.67)
?integrate


hazard <- function(x) {dnorm(x, 0.67, 0.16)}
integrate(hazard, 0, 0.67)

curve(dgamma(x, 4, 2), xlim = c(0, 5))
curve(dgamma(x, 2, 0.5), xlim = c(0, 5))

dens(rexp(10000, 1) + rexp(1000, 1))
dens(rgamma(10000, 2, 1))
mean(rexp(10000, 1) + rexp(10000, 1))
mean(rgamma(10000, 2, 1))

dens(rgamma(1000, 25, 0.8) + rgamma(1000, 25, 0.2))
dens(rgamma(1000, 25, 0.8))
dens(rgamma(1000, 25, 0.2))

dens(rgamma(10000, 5, 1))
dens(rgamma(1000, 5, 1) + rgamma(1000, 5, 1))

dens(rgamma(10000, 10, 2))

mean(rgamma(10000, 10, 2))
mean(rgamma(10000, 5, 1))

# If individuals can only move one at a time, 
# we add the exponential distributions for each:
mean(25 * rexp(10000, 0.8) + 25 * rexp(10000, 0.2))
dens(25 * rexp(10000, 0.8) + 25 * rexp(10000, 0.2))

# This should be equivalent to a gamma distribution, with 25 events:
mean(rgamma(10000, 25, 0.8) + rgamma(10000,25, 0.2))
dens(rgamma(10000,25, 0.8) + rgamma(10000,25, 0.2))
# (Due to repeated sampling, the uncertainty in the latter is higher.)

#However, if individuals can move simultaneously, we can add the rates, too.
#(can we really add them? After all, once an individual has moved, this rate drops out).
mean(rgamma(10000, 25, 25*0.8) + rgamma(10000,25, 25*0.2))
dens(rgamma(10000,25, 25*0.8) + rgamma(10000,25, 25*0.2))
# A completely different situation.
# We should also not add the two distributions, because also those events happen
# simultaneously:
mean(pmax(apply(matrix(rep(rexp(10000, 0.8), 25), ncol = 10000), 2, max), apply(matrix(rep(rexp(10000, 0.2), 25), ncol = 10000), 2, max)))
dens(pmax(apply(matrix(rep(rexp(10000, 0.8), 25), ncol = 10000), 2, max), apply(matrix(rep(rexp(10000, 0.2), 25), ncol = 10000), 2, max)))


myM <- matrix(rep(rexp(1000, 0.8), 25), ncol = 1000)
apply(myM, 2, max)

apply(matrix(rep(rexp(1000, 0.8), 25), ncol = 1000), 2, max)
max

dens()


curve(dgamma(x, 5, 0.5) * dgamma(x, 5, 1), xlim = c(0, 10))
curve(dgamma(x, 5, 1), xlim = c(0, 10))
curve(dgamma(x, 5, 0.5), xlim = c(0, 10))

polyex <- function (x){(dexp(x, 0.2) * dexp(x, 0.2) *dexp(x, 0.2) )}

curve(dexp(x, 0.2))

#################################################
#10-11-2020

#Time t until all individuals have moved, discrete time, individuals waiting for one another
sum(rgeom(25, 0.2)) + sum(rgeom(25, 0.8))
# Distribution of this
dens(mapply(function(x1, x2){ sum(rgeom(25, x1)) + sum(rgeom(25, x2))},rep(0.2, 10000), rep(0.8, 10000)), adj = 0.8, xlim = c(0, 200), ylim = c(0, 0.08))

#Time t until all individuals have moved, continuous time, simultaneous movement
max(c(rexp(25, 0.2), rexp(25, 0.8)))
#Distribution of this
dens(mapply(function(x1, x2){ max(c(rexp(25, x1), rexp(25, x2)))},rep(0.2, 10000), rep(0.8, 10000)), adj = 0.8, add = TRUE)
















