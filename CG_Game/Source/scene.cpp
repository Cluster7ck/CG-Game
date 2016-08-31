#include "../Header/scene.h"

void traverseHierarchy(SceneNode* node);

Scene::Scene() {
	this->m_Root.setName("Root");
	this->m_Root.setParent(NULL);
}

Scene::~Scene() {
}

bool Scene::addSceneFile(const char* scenefile) {
	std::ifstream fileStream(scenefile);
	if (!fileStream) {
		std::cout << "Die Datei \"" << scenefile << "\" kann nicht geoeffnet werden." << std::endl;
		exit(-1);
	}

	std::map<std::string, Model*>::iterator map_iter;
	std::string lastNodeID("");
	SceneNode* lastSceneNode = NULL;

	std::string line = "";
	while (std::getline(fileStream, line)) {
		if (line[0] == '\0' || line[0] == '#' || line.empty()) {
			continue;
		}
		const char *charPointer = line.c_str();

		if (strncmp(charPointer, "MODEL", 5) == 0) {
			char Modelfile[256];
			char ModelID[256];
			char fullFilename[256];

			sscanf(charPointer, "MODEL ID=%s FILE=%s", ModelID, Modelfile);
			Model* n_Model = new Model();
			std::string modelString(ModelID);
			replaceFilename(scenefile, Modelfile, fullFilename);
			n_Model->load(fullFilename);
			m_Models.emplace(modelString, n_Model);
		}
		else if (strncmp(charPointer, "NODE", 4) == 0) {
			Vector Pos, Scale, RotAxis;
			float Angle;
			char NodeID[256];
			char ParentID[256];
			char ModelID[256];

			sscanf(charPointer, "NODE ID=%s PARENTID=%s MODELID=%s TRANSLATION=%f %f %f ROTATIONAXIS=%f %f %f ROTATIONANGLE = %f SCALE = %f %f %f",
				NodeID, ParentID, ModelID,
				&Pos.X, &Pos.Y, &Pos.Z,
				&RotAxis.X, &RotAxis.Y, &RotAxis.Z,
				&Angle,
				&Scale.X, &Scale.Y, &Scale.Z);

			map_iter = m_Models.find(std::string(ModelID));
			if (map_iter != m_Models.end()) {
				if (strncmp(ParentID, "NULL", 4) == 0) {
					SceneNode* n_SceneNode = new SceneNode(NodeID, Pos, RotAxis, Angle, Scale, &m_Root, map_iter->second);
					m_Root.addChild(n_SceneNode);
					lastSceneNode = n_SceneNode;
				}
				else {
					if (std::string(NodeID).compare(lastNodeID)) {
						SceneNode* n_SceneNode = new SceneNode(NodeID, Pos, RotAxis, Angle, Scale, lastSceneNode, map_iter->second);
						lastSceneNode->addChild(n_SceneNode);
						lastSceneNode = n_SceneNode;
					}
					else{
						exit(1);
						//TODO Traverse and search
					}
				}
			}
			//SceneNode* n_SceneNode = new SceneNode(NodeID,Pos,RotAxis,Angle,Scale,;
			lastNodeID = std::string(NodeID);
		}

	}
	//traverseHierarchy(&(this->m_Root));
	return true;
}

void Scene::draw() {
	std::set<SceneNode *>::iterator it;

	for (it = this->m_Root.getChildren().begin(); it != this->m_Root.getChildren().end(); ++it) {
		this->draw(*it);
	}
}

void Scene::draw(SceneNode *node) {
	if (node != NULL && node->getModel() != NULL) {
		glPushMatrix();
		glMultMatrixf(node->getGlobalTransform());
		node->getModel()->drawBuffer();
		glPopMatrix();
	}

	std::set<SceneNode *>::iterator it;
	for (it = node->getChildren().begin(); it != node->getChildren().end(); ++it) {
		this->draw(*it);
	}
}

void traverseHierarchy(SceneNode* node) {
	static int i = 0;
	std::set<SceneNode *>::iterator it;
	for (it = node->getChildren().begin(); it != node->getChildren().end(); ++it) {
		std::cout << node->getName() << std::endl;
		std::cout << i++ << std::endl;
		traverseHierarchy(*it);
	}
}