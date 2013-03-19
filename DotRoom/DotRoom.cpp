// DotRoom.cpp : main project file.

#include "stdafx.h"


int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Must pass two (2) and only two parameters to the program.\n");
		return 1;
	}

	FILE *sceneFile;
	sceneFile = fopen(argv[1], "r");
	if (!sceneFile)
	{
		printf("Could not open the .scene file provided by the first parameter. Aborting.\n");
		return 1;
	}

	fseek(sceneFile, 0, SEEK_END);
	int sceneFileLength = ftell(sceneFile);
	fseek(sceneFile, 0, SEEK_SET);

	char *sceneFileXml = new char[sceneFileLength + 1];
	memset(sceneFileXml, 0, sceneFileLength + 1);
	if (fread(sceneFileXml, sizeof(char), sceneFileLength, sceneFile) != sizeof(char) * sceneFileLength && !feof(sceneFile))
	{
		fclose(sceneFile);
		printf("Error reading the .scene file. Aborting.\n");
		return 1;
	}
	fclose(sceneFile);
	xml_document<> scene;
	scene.parse<0>(sceneFileXml);

	FILE *roomFile;
	roomFile = fopen(argv[2], "wb");

	xml_node<> *nodes = scene.first_node("scene")->first_node("nodes");
	int numberNodes = 0;
	for (xml_node<> *node = nodes->first_node("node"); node; node = node->next_sibling("node"))
	{
		numberNodes++;
	}

	if (!numberNodes)
	{
		fclose(roomFile);
		printf("Provided .scene file contains no nodes. Invalid, aborting.\n");
		return 1;
	}
	if (!fwrite(&numberNodes, sizeof(int), 1, roomFile))
	{
		fclose(roomFile);
		printf("Could not write data to .room file. Aborting.\n");
		return 1;
	}

	for (xml_node<> *node = nodes->first_node("node"); node; node = node->next_sibling("node"))
	{
		// Mesh
		char *meshName = node->first_node("entity")->first_attribute("meshFile")->value();
		int meshNameLength = strlen(meshName);
		printf("mesh: '%s' (%d).\n", meshName, meshNameLength);
		if (!fwrite(&meshNameLength, sizeof(int), 1, roomFile))
		{
			fclose(roomFile);
			printf("Could not write data to .room file. Aborting.\n");
			return 1;
		}
		if (fwrite(meshName, sizeof(char), meshNameLength, roomFile) != meshNameLength)
		{
			fclose(roomFile);
			printf("Could not write data to .room file. Aborting.\n");
			return 1;
		}

		// Position
		float position[3];
		position[0] = atof(node->first_node("position")->first_attribute("x")->value());
		position[1] = atof(node->first_node("position")->first_attribute("y")->value());
		position[2] = atof(node->first_node("position")->first_attribute("z")->value());
		printf("'%s': {%.4f, %.4f, %.4f}\n", meshName, position[0], position[1], position[2]);
		if (fwrite(position, sizeof(float), 3, roomFile) != 3)
		{
			fclose(roomFile);
			printf("Could not write data to .room file. Aborting.\n");
			return 1;
		}

		// Scale
		float scale[3];
		scale[0] = atof(node->first_node("scale")->first_attribute("x")->value());
		scale[1] = atof(node->first_node("scale")->first_attribute("y")->value());
		scale[2] = atof(node->first_node("scale")->first_attribute("z")->value());
		printf("'%s': {%.4f, %.4f, %.4f}\n", meshName, scale[0], scale[1], scale[2]);
		if (fwrite(scale, sizeof(float), 3, roomFile) != 3)
		{
			fclose(roomFile);
			printf("Could not write data to .room file. Aborting.\n");
			return 1;
		}

		// Rotation
		float rotation[4];
		rotation[0] = atof(node->first_node("rotation")->first_attribute("qw")->value());
		rotation[1] = atof(node->first_node("rotation")->first_attribute("qx")->value());
		rotation[2] = atof(node->first_node("rotation")->first_attribute("qy")->value());
		rotation[3] = atof(node->first_node("rotation")->first_attribute("qz")->value());
		printf("'%s': {%.4f, %.4f, %.4f, %.4f}\n", meshName, rotation[0], rotation[1], rotation[2], rotation[3]);
		if (fwrite(rotation, sizeof(float), 4, roomFile) != 4)
		{
			fclose(roomFile);
			printf("Could not write data to .room file. Aborting.\n");
			return 1;
		}
	}

	fclose(roomFile);
    return 0;
}
