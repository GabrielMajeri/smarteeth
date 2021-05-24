#include "directions.hpp"

#include "brushing.hpp"
#include "configuration.hpp"

std::pair<LedsColor, Direction> getDirection()
{
    const Configuration &config = *getCurrentConfiguration();

    return std::make_pair(LedsColor::Red, Direction::Turn_Off);
}

std::vector<vector<unsigned>> getDirections(Brushing &brush, int index, int &nb)
{
    std::vector<int>::iterator it = std::find(bleeds.begin(), bleeds.end(), index);
    if (it == bleeds.end())
    {
        //dintele sangereaza
        brush.Leds.push_back("Red");
        brush.Graphic_Directions.push_back("X");
    }
    if ((index == 1 || index == 32) || (index > 1 && index <= 8) || (index >= 25 && index < 32)) //ma aflu la primul dinte de sus sau la ultimul de jos deci trebuie sa merg spre stanga
    {
        vector<vector<unsigned>> matrix{
            {0, 0, 0, 0, 1, 0, 0, 0},
            {0, 0, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0, 0, 1, 0},
            {1, 1, 1, 1, 1, 1, 1, 1},
            {0, 0, 0, 0, 0, 0, 1, 0},
            {0, 0, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0}};
        nb ++;
        brush.Leds.push_back("Purple");
        brush.Graphic_Directions.push_back("==>");
        brush.Directions.push_back("Right");
        return matrix;
    }
    if ((index == 16 || index == 17) || (index > 17 && index <= 24) || (index >= 9 && index <= 16)) //ma aflu la ultimul dinte de sus sau la primul de jos, deci dreapta
    {
        vector<vector<unsigned>> matrix{
            {0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 1, 0, 0, 0, 0, 0},
            {0, 1, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 1, 1, 1},
            {0, 1, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0, 0}};
        nb++;
        brush.Leds.push_back("Blue");
        brush.Graphic_Directions.push_back("<==");
        brush.Directions.push_back("Left");
        return matrix;
    }
}

std::string getDirectionsRT(Brushing &brush, unsigned index, unsigned th1, unsigned th2)
{
    std::string returnString = "";
    returnString += "Tooth ";
    returnString += index + "\n";
    int nb = 0; //numarul pe care l are dintele in vectorii de directie de leduri etc

    vector<vector<unsigned>> mtx = getDirections(brush, index, nb);

    returnString += "Led " + brush.Leds[nb];
    returnString += "\n Direction " + brush.Directions[nb];
    returnString += "\n Graphic Directions " + brush.Graphic_Directions[nb] + "\n";

    for (unsigned j = 0; j < 7; j++)
    {
        for (unsigned k = 0; k < 8; k++)
        {
            if (mtx[j][k])
                returnString += "* ";
            else
                returnString += " ";
        }
        returnString += "\n";
    }

    return returnString;
}    

void setDirections(Config &configuration)
{
    selectUserStats(configuration.stats);

    Brushing *currentBrushing = new Brushing();
    std::cout << "User name:" << configuration.Name << '\n';
    std::string returnstring = "";
    std::vector<int> verif(configuration.STeeth.size(), 0);
    startCronometru(); //porneste cronometrul pentru periere

    if ((int)configuration.program == 1 || (int)configuration.program == 2)
    {
        //periere completa sau de sus, incep dinte 9->16 apoi 1->8, 24->17, 25->32
        for (unsigned i = 9; i <= 16; i++)
            returnstring += getDirectionsRT(*currentBrushing, i, 9, 16);
        for (unsigned i = 8; i <= 1; i--)
        {
            //8 -> 1
            returnstring += getDirectionsRT(*currentBrushing, i, 8, 1);
        }
    }
    if ((int)configuration.program == 1 || (int)configuration.program == 3)
    {
        //periati dintii de jos acum 24->17
        for (unsigned i = 24; i >= 17; i--)
        {
            returnstring += getDirectionsRT(*currentBrushing, i, 24, 17);
        }
        for (unsigned i = 25; i <= 32; i--)
        {
            //25->32
            returnstring += getDirectionsRT(*currentBrushing, i, 25, 32);
        }
    }
    if ((int)configuration.program == 4)
    {
        currentBrushing->Leds.push_back("Grey");
    }

    std::string Statistics = statistics();
    saveUserStats(configuration.stats);
    returnstring += Statistics;
    currentBrushing->History.push_back(returnstring);
    saved_Brushing.push_back(currentBrushing);
}

void getDirections(const Rest::Request &request, Http::ResponseWriter response)
{
    std::string returnString = "No brushing available!";
    unsigned j = 0;
    for (Brushing *&currBrushing : saved_Brushing)
    {
        returnString = "";
        for (unsigned i = 0; i < currBrushing->Directions.size(); ++i)
        {
            returnString += "Colour is ";
            returnString += currBrushing->Leds[i];
            returnString += ", direction is ";
            returnString += currBrushing->Directions[i];
            returnString += currBrushing->Graphic_Directions[i]; //pt ca bratara
            returnString += '\n';
            returnString += "================================";
        }
        returnString += currBrushing->History[j];
        returnString += '\n';
        returnString += "======================================= \n";
    }

    response.send(Http::Code::Ok, returnString.c_str());
}
