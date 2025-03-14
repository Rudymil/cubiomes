#include "finders.h"
#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <windows.h>

// Tableau des noms des structures
const char* structureNames[] = {
    "Feature",
    "Pyramide du désert",
    "Temple de la jungle",
    "Cabane de sorcière",
    "Igloo",
    "Village",
    "Ruine océanique",
    "Épave",
    "Monument sous-marin",
    "Manoir",
    "Avant-poste",
    "Portail en ruine",
    "Portail en ruine (Nether)",
    "Cité ancienne",
    "Trésor",
    "Mine abandonnée",
    "Puit du désert",
    "Géode",
    "Forteresse",
    "Bastion",
    "Cité de l'End",
    "Portail de l'End",
    "Île de l'End",
    "Ruine de sentier",
    "Chambre des épreuves"
};

// Fonction pour obtenir l'icône en fonction du type de structure
const char* getStructureIcon(int structureType)
{
    switch (structureType)
    {
        case Village: return "house";
        case Desert_Pyramid: return "temple";
        case Jungle_Temple: return "temple";
        case Swamp_Hut: return "door";
        case Igloo: return "blueflag";
        case Ocean_Ruin: return "tower";
        case Shipwreck: return "anchor";
        case Monument: return "temple";
        case Mansion: return "bighouse";
        case Outpost: return "lighthouse";
        case Ruined_Portal: return "portal";
        case Ancient_City: return "skull";
        case Treasure: return "chest";
        case Mineshaft: return "minecart";
        case Desert_Well: return "drink";
        case Geode: return "diamond";
        case Trail_Ruins: return "tower";
        case Trial_Chambers: return "key";
        default: return "default";
    }
}

// Fonction pour trouver des structures dans une zone spécifiée
void findStructures(FILE *file, int structureType, int mc, int dim, uint64_t seed,
    int x0, int z0, int x1, int z1)
{
    // Initialisation du générateur de biomes
    Generator g;
    setupGenerator(&g, mc, 0);
    applySeed(&g, dim, seed);

    // Configuration de la structure
    StructureConfig sconf;
    if (!getStructureConfig(structureType, mc, &sconf))
        return; // Version ou structure incorrecte

    // Segmentation de la zone en régions de structure
    double blocksPerRegion = sconf.regionSize * 16.0;
    int rx0 = (int) floor(x0 / blocksPerRegion);
    int rz0 = (int) floor(z0 / blocksPerRegion);
    int rx1 = (int) ceil(x1 / blocksPerRegion);
    int rz1 = (int) ceil(z1 / blocksPerRegion);
    int i, j;

    // Parcours des régions de structure
    for (j = rz0; j <= rz1; j++)
    {
        for (i = rx0; i <= rx1; i++)
        {   
            // Vérification de la tentative de génération de structure dans la région (i, j)
            Pos pos;
            if (!getStructurePos(structureType, mc, seed, i, j, &pos))
                continue; // Cette région n'est pas adaptée
            if (pos.x < x0 || pos.x > x1 || pos.z < z0 || pos.z > z1)
                continue; // La structure est en dehors de la zone spécifiée
            if (!isViableStructurePos(structureType, &g, pos.x, pos.z, 0))
                continue; // Les biomes ne sont pas viables
            else if (mc >= MC_1_18)
            {   
                // Certaines structures dans 1.18+ dépendent du terrain
                if (!isViableStructureTerrain(structureType, &g, pos.x, pos.z))
                    continue;
            }
            // Affichage des coordonnées de la structure trouvée
            printf("Structure %s trouvée à : %d, %d (icon: %s)\n", structureNames[structureType], pos.x, pos.z, getStructureIcon(structureType));
            // Écriture des coordonnées de la structure trouvée dans le fichier CSV
            fprintf(file, "%s,%d,%d,%s\n", structureNames[structureType], pos.x, pos.z, getStructureIcon(structureType));
        }
    }
}

// Fonction principale
int main()
{
    // Définition du seed et du rayon de recherche
    uint64_t seed = -4357231103345683077LL;
    int r = 10000;

    // Variables pour la version de Minecraft et la dimension
    int mcVersion = MC_1_21_3;
    int dimension = DIM_OVERWORLD;

    // Liste des types de structures à rechercher
    int structures[] = {
        Village,
        Desert_Pyramid,
        Jungle_Temple,
        Swamp_Hut,
        Igloo,
        Ocean_Ruin,
        Shipwreck,
        Monument,
        Mansion,
        Outpost,
        Ruined_Portal,
        Ancient_City,
        Treasure,
        // Mineshaft,
        Desert_Well,
        // Geode,
        Trail_Ruins,
        Trial_Chambers
    };

    // Configuration de la locale pour utiliser UTF-8
    setlocale(LC_ALL, "");
    // Configuration de la console pour utiliser UTF-8
    SetConsoleOutputCP(CP_UTF8);

    // Ouverture du fichier CSV pour écrire les résultats
    FILE *file = fopen("export_markers_dynmap.csv", "w");
    if (!file)
    {
        perror("Échec de l'ouverture du fichier");
        return 1;
    }

    // Écriture de l'en-tête du fichier CSV
    fprintf(file, "structureType,X,Z,icon\n");

    // Affichage des structures dans un rayon de r blocs
    printf("Structures dans un rayon de %d blocs (seed : %lld, version : %d, dimension : %d) :\n", r, seed, mcVersion, dimension);

    // Parcours de la liste des structures et appels à la fonction findStructures
    for (int i = 0; i < sizeof(structures) / sizeof(structures[0]); i++)
    {
        findStructures(file, structures[i], mcVersion, dimension, seed, -r, -r, +r, +r);
    }

    // Fermeture du fichier CSV
    fclose(file);

    return 0;
}