# code to plot botero replicates
using CSV
using DataFrames
using Gadfly

# function for cues
function process_cue(R, P, h, I01, I02, b1, b2, s, a)
    data_rnorm = DataFrame()
    if s <= 0.5
        if h <= 1
            data_rnorm = DataFrame(
                intercept = [I01 * h, I02 * (1 - h)],
                slope = [0, 0],
                strategy = ["dbh1", "dbh2"]
            )
        else
            data_rnorm = DataFrame(
                intercept = [I01],
                slope = 0,
                strategy = "cbh"
            )
        end
    else
        if h <= 1
            data_rnorm = DataFrame(
                intercept = [I02],
                slope = [b2],
                strategy = (a < 0 ? ["dev_plast"] : ["fen_plast"])
            )
        else
            data_rnorm = DataFrame(
                intercept = [0],
                slope = [b1],
                strategy = (a < 0 ? ["dev_plast"] : ["fen_plast"])
            )
        end
    end
    data_rnorm[!, :R] .= R
    data_rnorm[!, :P] .= P
    return data_rnorm
end

# test function
process_cue(33, 23, 0.2, 1, 0.2, 1, 0.4, 0.4, 0.3)

# read in data of evolved values
data = CSV.read("data/AverageValues_Seed_1.csv", DataFrame)

responses = vcat(
    process_cue.(data.R, data.P, data.h, data.I01, data.I02,
        data.b1, data.b2, data.s, data.a)...
)

# use ggplot because always better
using RCall

R"library(ggplot2)"

R"ggplot($responses)+
    geom_abline(aes(intercept = intercept, slope = slope, col = strategy))+
    scale_x_continuous(limits = c(0,1))+
    scale_y_continuous(limits = c(0,1))+
    coord_cartesian(xlim=c(0,1), ylim=c(0, 1))+
    facet_grid(P~R, labeller = label_both, as.table = F,
                switch = 'both')+
    theme_bw()"