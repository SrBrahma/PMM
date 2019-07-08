To make these same arrays, the "GeomagnetismLibrary.cpp" file should be replaced into the
WMM2015v2 software (https://www.ngdc.noaa.gov/geomag/WMM/soft.shtml) src directory, and you must re-make it.
On future updates of the WMM software, my small changes may not work anymore. If I don't update the git, you should be
able to replicate the changes by yourself, by comparing the original functions with my changed one. It's easy, I promise.

The only changed functions are below, already changed:





// Changed by Henrique Bruno, to output in a C array format.
int MAG_Grid(MAGtype_CoordGeodetic minimum, MAGtype_CoordGeodetic maximum, double
        cord_step_size, double altitude_step_size, double time_step, MAGtype_MagneticModel *MagneticModel, MAGtype_Geoid
        *Geoid, MAGtype_Ellipsoid Ellip, MAGtype_Date StartDate, MAGtype_Date EndDate, int ElementOption, int UncertaintyOption, int PrintOption, char *OutputFile)
{
    int NumTerms;
    double b, c, PrintElement, ErrorElement = 0;
    
    int totalLatitudes = 0, totalLongitudes = 0;

    MAGtype_MagneticModel *TimedMagneticModel;
    MAGtype_CoordSpherical CoordSpherical;
    MAGtype_MagneticResults MagneticResultsSph, MagneticResultsGeo, MagneticResultsSphVar, MagneticResultsGeoVar;
    MAGtype_SphericalHarmonicVariables *SphVariables;
    MAGtype_GeoMagneticElements GeoMagneticElements, Errors;
    MAGtype_LegendreFunction *LegendreFunction;
    MAGtype_Gradient Gradient;
    
    FILE *fileout = NULL;

    if(PrintOption == 1)
    {
        fileout = fopen(OutputFile, "w");
        if(!fileout)
        {
            printf("Error opening %s to write", OutputFile);
            return FALSE;
        }
    }



    if(fabs(cord_step_size) < 1.0e-10) cord_step_size = 99999.0; /*checks to make sure that the step_size is not too small*/
    if(fabs(altitude_step_size) < 1.0e-10) altitude_step_size = 99999.0;
    if(fabs(time_step) < 1.0e-10) time_step = 99999.0;


    NumTerms = ((MagneticModel->nMax + 1) * (MagneticModel->nMax + 2) / 2);
    TimedMagneticModel = MAG_AllocateModelMemory(NumTerms);
    LegendreFunction = MAG_AllocateLegendreFunctionMemory(NumTerms); /* For storing the ALF functions */
    SphVariables = MAG_AllocateSphVarMemory(MagneticModel->nMax);

    b = minimum.phi;
    c = minimum.lambda;


    for(minimum.phi = b; minimum.phi <= maximum.phi; minimum.phi += cord_step_size, totalLatitudes++);
    for(minimum.lambda = c; minimum.lambda <= maximum.lambda; minimum.lambda += cord_step_size, totalLongitudes++);

    // By Henrique Bruno.
    if(PrintOption == 1) fprintf(fileout, "int16_t resultingArray[%i][%i] = {\n", totalLatitudes, totalLongitudes);
    else printf("int16_t resultingArray[%i][%i] = {\n", totalLatitudes, totalLongitudes);
    // -=-=-=-=-=-

    for(minimum.phi = b; minimum.phi <= maximum.phi; minimum.phi += cord_step_size) /*Latitude loop*/
    {
        if(PrintOption == 1) fprintf(fileout, "    { ");
        else printf("    { ");

        for(minimum.lambda = c; minimum.lambda <= maximum.lambda; minimum.lambda += cord_step_size) /*Longitude loop*/
        {
            if(Geoid->UseGeoid == 1)
                MAG_ConvertGeoidToEllipsoidHeight(&minimum, Geoid); /* This converts the height above mean sea level to height above the WGS-84 ellipsoid */
            else
                minimum.HeightAboveEllipsoid = minimum.HeightAboveGeoid;
            MAG_GeodeticToSpherical(Ellip, minimum, &CoordSpherical);
            MAG_ComputeSphericalHarmonicVariables(Ellip, CoordSpherical, MagneticModel->nMax, SphVariables); /* Compute Spherical Harmonic variables  */
            MAG_AssociatedLegendreFunction(CoordSpherical, MagneticModel->nMax, LegendreFunction); /* Compute ALF  Equations 5-6, WMM Technical report*/

            MAG_TimelyModifyMagneticModel(StartDate, MagneticModel, TimedMagneticModel); /*This modifies the Magnetic coefficients to the correct date. */
            MAG_Summation(LegendreFunction, TimedMagneticModel, *SphVariables, CoordSpherical, &MagneticResultsSph); /* Accumulate the spherical harmonic coefficients Equations 10:12 , WMM Technical report*/
            MAG_SecVarSummation(LegendreFunction, TimedMagneticModel, *SphVariables, CoordSpherical, &MagneticResultsSphVar); /*Sum the Secular Variation Coefficients, Equations 13:15 , WMM Technical report  */
            MAG_RotateMagneticVector(CoordSpherical, minimum, MagneticResultsSph, &MagneticResultsGeo); /* Map the computed Magnetic fields to Geodetic coordinates Equation 16 , WMM Technical report */
            MAG_RotateMagneticVector(CoordSpherical, minimum, MagneticResultsSphVar, &MagneticResultsGeoVar); /* Map the secular variation field components to Geodetic coordinates, Equation 17 , WMM Technical report*/
            MAG_CalculateGeoMagneticElements(&MagneticResultsGeo, &GeoMagneticElements); /* Calculate the Geomagnetic elements, Equation 18 , WMM Technical report */
            MAG_CalculateGridVariation(minimum, &GeoMagneticElements);
            MAG_CalculateSecularVariationElements(MagneticResultsGeoVar, &GeoMagneticElements); /*Calculate the secular variation of each of the Geomagnetic elements, Equation 19, WMM Technical report*/
            MAG_WMMErrorCalc(GeoMagneticElements.H, &Errors);
            
            if(ElementOption >= 17)
                MAG_Gradient(Ellip, minimum, TimedMagneticModel, &Gradient);

            switch(ElementOption) {
                case 1:
                    PrintElement = GeoMagneticElements.Decl; /*1. Angle between the magnetic field vector and true north, positive east*/
                    ErrorElement = Errors.Decl;
                    break;
                case 2:
                    PrintElement = GeoMagneticElements.Incl; /*2. Angle between the magnetic field vector and the horizontal plane, positive downward*/
                    ErrorElement = Errors.Incl;                            
                    break;
                case 3:
                    PrintElement = GeoMagneticElements.F; /*3. Magnetic Field Strength*/
                    ErrorElement = Errors.F;
                    break;
                case 4:
                    PrintElement = GeoMagneticElements.H; /*4. Horizontal Magnetic Field Strength*/
                    ErrorElement = Errors.H;
                    break;
                case 5:
                    PrintElement = GeoMagneticElements.X; /*5. Northern component of the magnetic field vector*/
                    ErrorElement = Errors.X;
                    break;
                case 6:
                    PrintElement = GeoMagneticElements.Y; /*6. Eastern component of the magnetic field vector*/
                    ErrorElement = Errors.Y;
                    break;
                case 7:
                    PrintElement = GeoMagneticElements.Z; /*7. Downward component of the magnetic field vector*/
                    ErrorElement = Errors.Z;
                    break;
                case 8:
                    PrintElement = GeoMagneticElements.GV; /*8. The Grid Variation*/
                    ErrorElement = Errors.Decl;
                    break;
                case 9:
                    PrintElement = GeoMagneticElements.Decldot * 60; /*9. Yearly Rate of change in declination*/
                    UncertaintyOption = 0;
                    break;
                case 10:
                    PrintElement = GeoMagneticElements.Incldot * 60; /*10. Yearly Rate of change in inclination*/
                    UncertaintyOption = 0;
                    break;
                case 11:
                    PrintElement = GeoMagneticElements.Fdot; /*11. Yearly rate of change in Magnetic field strength*/
                    UncertaintyOption = 0;
                    break;
                case 12:
                    PrintElement = GeoMagneticElements.Hdot; /*12. Yearly rate of change in horizontal field strength*/
                    UncertaintyOption = 0;
                    break;
                case 13:
                    PrintElement = GeoMagneticElements.Xdot; /*13. Yearly rate of change in the northern component*/
                    UncertaintyOption = 0;
                    break;
                case 14:
                    PrintElement = GeoMagneticElements.Ydot; /*14. Yearly rate of change in the eastern component*/
                    UncertaintyOption = 0;
                    break;
                case 15:
                    PrintElement = GeoMagneticElements.Zdot; /*15. Yearly rate of change in the downward component*/
                    UncertaintyOption = 0;
                    break;
                case 16:
                    PrintElement = GeoMagneticElements.GVdot;
                    UncertaintyOption = 0;
                    /*16. Yearly rate of change in grid variation*/;
                    break;
                case 17:
                    PrintElement = Gradient.GradPhi.X;
                    UncertaintyOption = 0;
                    break;
                case 18:
                    PrintElement = Gradient.GradPhi.Y;
                    UncertaintyOption = 0;
                    break;
                case 19:
                    PrintElement = Gradient.GradPhi.Z;
                    UncertaintyOption = 0;
                    break;
                case 20:
                    PrintElement = Gradient.GradLambda.X;
                    UncertaintyOption = 0;
                    break;
                case 21:
                    PrintElement = Gradient.GradLambda.Y;
                    UncertaintyOption = 0;
                    break;
                case 22:
                    PrintElement = Gradient.GradLambda.Z;
                    UncertaintyOption = 0;
                    break;
                case 23:
                    PrintElement = Gradient.GradZ.X;
                    UncertaintyOption = 0;
                    break;
                case 24:
                    PrintElement = Gradient.GradZ.Y;
                    UncertaintyOption = 0;
                    break;
                case 25:
                    PrintElement = Gradient.GradZ.Z;
                    UncertaintyOption = 0;
                    break;
                default:
                    PrintElement = GeoMagneticElements.Decl; /* 1. Angle between the magnetic field vector and true north, positive east*/
                    ErrorElement = Errors.Decl;
            }

            if(PrintOption == 1) fprintf(fileout, "%i", (int)(PrintElement*100));
            else printf("%i", (int)(PrintElement*100));

            

            if(minimum.lambda + cord_step_size <= maximum.lambda) // If we still have longitudes at this latitude, write a ", "!
            {
                if(PrintOption == 1) fprintf(fileout, ", ");
                else printf(", ");
            }

            else    // Henrique Bruno: We finished a latitude!
            {
                if(minimum.phi + cord_step_size <= maximum.phi) // If we still have another latitudes, write a ", "!
                {
                    if(PrintOption == 1) fprintf(fileout, "},\n");
                    else printf("},\n");
                }

                else
                {
                    if(PrintOption == 1) fprintf(fileout, "}\n");
                    else printf("}\n");
                }
            }

        } /*Longitude Loop */

    } /* Latitude Loop */

    if(PrintOption == 1) fprintf(fileout, "};");
    else printf("};");

    if(PrintOption == 1) fclose(fileout);


    MAG_FreeMagneticModelMemory(TimedMagneticModel);
    MAG_FreeLegendreMemory(LegendreFunction);
    MAG_FreeSphVarMemory(SphVariables);

    return TRUE;
} /*MAG_Grid*/






int MAG_GetUserGrid(MAGtype_CoordGeodetic *minimum, MAGtype_CoordGeodetic *maximum, double *step_size, double *a_step_size, double *step_time, MAGtype_Date
        *StartDate, MAGtype_Date *EndDate, int *ElementOption, int *PrintOption, char *OutputFile, MAGtype_Geoid *Geoid)

/* Prompts user to enter parameters to compute a grid - for use with the MAG_grid function
Note: The user entries are not validated before here. The function populates the input variables & data structures.

UPDATE : minimum Pointer to data structure with the following elements
                double lambda; (longitude)
                double phi; ( geodetic latitude)
                double HeightAboveEllipsoid; (height above the ellipsoid (HaE) )
                double HeightAboveGeoid;(height above the Geoid )

                maximum   -same as the above -MAG_USE_GEOID
                step_size  : double pointer : spatial step size, in decimal degrees
                a_step_size : double pointer :  double altitude step size (km)
                step_time : double pointer : time step size (decimal years)
                StartDate : pointer to data structure with the following elements updates
                                        double DecimalYear;     ( decimal years )
                EndDate :	Same as the above
CALLS : none


 */
{
    FILE *fileout;
    char filename[] = "GridProgramDirective.txt";
    char buffer[20];

    printf("Program slightly modified by Henrique Bruno. The results are multiplied by 100, and removed the decimal part.\n");

    strcpy(buffer, "");
    printf("Do you want to automatically set the Latitude range as [-90, 90] and the Longitude range as [-180, 180]? (y/n)\n");
    if (NULL == fgets(buffer, 20, stdin)) {
        minimum->phi    =  -90;
        maximum->phi    =   90;
        minimum->lambda = -180;
        maximum->lambda =  180;
        printf("Unrecognized input. Default YES used.\n");
    } else {
        sscanf(buffer, "%lf", &minimum->phi);
    }
    
    if (buffer[0] == 'y' || buffer[0] == 'Y')
    {
        minimum->phi    =  -90;
        maximum->phi    =   90;
        minimum->lambda = -180;
        maximum->lambda =  180;
    }
    else
    {
        printf("Please Enter Minimum Latitude (in decimal degrees):\n");
        if (NULL == fgets(buffer, 20, stdin)) {
            minimum->phi = 0;
            printf("Unrecognized input default %lf used\n", minimum->phi);
        }else {
            sscanf(buffer, "%lf", &minimum->phi);
        }
        strcpy(buffer, "");
        printf("Please Enter Maximum Latitude (in decimal degrees):\n");
        if (NULL == fgets(buffer, 20, stdin)) {
            maximum->phi = 0;
            printf("Unrecognized input default %lf used\n", maximum->phi);
        } else {
            sscanf(buffer, "%lf", &maximum->phi);
        }
        strcpy(buffer, "");
        printf("Please Enter Minimum Longitude (in decimal degrees):\n");
        if (NULL == fgets(buffer, 20, stdin)) {
            minimum->lambda = 0;
            printf("Unrecognized input default %lf used\n", minimum->lambda);
        } else {
            sscanf(buffer, "%lf", &minimum->lambda);
        }
        strcpy(buffer, "");
        printf("Please Enter Maximum Longitude (in decimal degrees):\n");
        if (NULL == fgets(buffer, 20, stdin)){
            maximum->lambda = 0;
            printf("Unrecognized input default %lf used\n", maximum->lambda);
        } else {
            sscanf(buffer, "%lf", &maximum->lambda);
        }
    }
    printf("\nLatitude range set as [%.2f, %.2f] and the Longitude range set as [%.2f, %.2f]\n", minimum->phi, maximum->phi, minimum->lambda, maximum->lambda);

    strcpy(buffer, "");
    printf("\nPlease Enter Step Size (in decimal degrees):\n");
    if (NULL == fgets(buffer, 20, stdin)){
        *step_size = fmax(maximum->phi - minimum->phi, maximum->lambda - minimum->lambda);
        printf("Unrecognized input default %lf used\n", *step_size);
    } else {
        sscanf(buffer, "%lf", step_size);
    }

    strcpy(buffer, "");
    printf("\nAutomatically using the WGS-84 Ellipsoid.\n");
    Geoid->UseGeoid = 0;
    printf("Height of WGS-84 Ellipsoid automatically set as 0.\n");
    minimum->HeightAboveGeoid = 0;
    minimum->HeightAboveEllipsoid = minimum->HeightAboveGeoid;
    maximum->HeightAboveGeoid = 0;
    maximum->HeightAboveEllipsoid = maximum->HeightAboveGeoid;
    *a_step_size = 0;

    strcpy(buffer, "");
    printf("\nPlease Enter the decimal year:\n");
    while (NULL == fgets(buffer, 20, stdin)) {
        printf("\nUnrecognized input, please re-enter a decimal year\n");
    }
    sscanf(buffer, "%lf", &StartDate->DecimalYear);
    EndDate->DecimalYear = StartDate->DecimalYear;
    *step_time = 0;

    strcpy(buffer, "");
    printf("\nEnter a geomagnetic element to print. Your options are:\n");
    printf(" 1. Declination	9.   Ddot\n 2. Inclination	10. Idot\n 3. F		11. Fdot\n 4. H		12. Hdot\n 5. X		13. Xdot\n 6. Y		14. Ydot\n 7. Z		15. Zdot\n 8. GV		16. GVdot\nFor gradients enter: 17\n");
    if (NULL == fgets(buffer, 20, stdin)) {
        *ElementOption = 1;
        printf("Unrecognized input, default of %d used\n", *ElementOption);
    }
    sscanf(buffer, "%d", ElementOption);
    strcpy(buffer, "");
    if(*ElementOption == 17)
    {
        printf("Enter a gradient element to print. Your options are:\n");
        printf(" 1. dX/dphi \t2. dY/dphi \t3. dZ/dphi\n");
        printf(" 4. dX/dlambda \t5. dY/dlambda \t6. dZ/dlambda\n");
        printf(" 7. dX/dz \t8. dY/dz \t9. dZ/dz\n");
        strcpy(buffer, "");
        if (NULL == fgets(buffer, 20, stdin)) {
            *ElementOption=1;
            printf("Unrecognized input, default of %d used\n", *ElementOption);
        } else {
            sscanf(buffer, "%d", ElementOption);
        }
        strcpy(buffer, "");
        *ElementOption+=16;
    }


    *PrintOption = 1;

    strcpy(buffer, "");
    fileout = fopen(filename, "a");

    printf("\nPlease enter output filename\nfor default ('GridResults.txt') press enter:\n");
    if(NULL==fgets(buffer, 20, stdin) || strlen(buffer) <= 1)
    {
        strcpy(OutputFile, "GridResults.txt");
        fprintf(fileout, "\nResults printed in: GridResults.txt\n");
        strcpy(OutputFile, "GridResults.txt");
    } else
    {
        sscanf(buffer, "%s", OutputFile);
        fprintf(fileout, "\nResults printed in: %s\n", OutputFile);
    }

    strcpy(buffer, "");

    fprintf(fileout, "Minimum Latitude: %f\t\tMaximum Latitude: %f\t\tStep Size: %f\nMinimum Longitude: %f\t\tMaximum Longitude: %f\t\tStep Size: %f\n", minimum->phi, maximum->phi, *step_size, minimum->lambda, maximum->lambda, *step_size);
    if(Geoid->UseGeoid == 1)
        fprintf(fileout, "Minimum Altitude above MSL: %f\tMaximum Altitude above MSL: %f\tStep Size: %f\n", minimum->HeightAboveGeoid, maximum->HeightAboveGeoid, *a_step_size);
    else
        fprintf(fileout, "Minimum Altitude above WGS-84 Ellipsoid: %f\tMaximum Altitude above WGS-84 Ellipsoid: %f\tStep Size: %f\n", minimum->HeightAboveEllipsoid, maximum->HeightAboveEllipsoid, *a_step_size);
    fprintf(fileout, "Starting Date: %f\t\tEnding Date: %f\t\tStep Time: %f\n\n\n", StartDate->DecimalYear, EndDate->DecimalYear, *step_time);
    fclose(fileout);
    return TRUE;
}