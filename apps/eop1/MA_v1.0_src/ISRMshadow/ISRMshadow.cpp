
#include <stdio.h>
#include <string>
#include <fstream>
#include <amqm/AMQManager.h>
#include <Utils.h>
#include "ISRMshadow.h"

using namespace amqm;
using namespace cms;
using namespace std;

ISRMShadow::ISRMShadow(int maxDetects) : planManager(5), detects(maxDetects) {
	amq.listen("updateMissionPlan", std::bind(&ISRMShadow::updateMissionPlan, this, _1), true);
	amq.listen("requestISRMDetections", std::bind(&ISRMShadow::handleDetectionsRequest, this, _1), true);
	amq.listen("recieveRDRDetections", std::bind(&ISRMShadow::handleRecieveRDRDetections, this, _1), true);
	amq.listen("recieveEOIRDetections", std::bind(&ISRMShadow::handleRecieveEOIRDetections, this, _1), true);
	amq.listen("updateConfig", std::bind(&ISRMShadow::handleUpdateConfig, this, _1), true);
	json j = Utils::loadDefaultConfig();
	processConfigContent(j);
	//collectDetects();
}

void ISRMShadow::processConfigContent(json j) {
	detects.setSize(j["maxDetects"]);
}

void ISRMShadow::handleUpdateConfig(json j) {
	json k = Utils::loadConfig(j);
	processConfigContent(k);
}

ISRMShadow::~ISRMShadow() {
	planManager.clearAll();
	detects.clearAll();
}

void ISRMShadow::run() {

}

void ISRMShadow::classify() {
}
void ISRMShadow::print() {
	for (const auto& key_value : detects.getMap()) {
		cout << "id: " << key_value.first << endl;
		cout << "lat: " << key_value.second->getLat() << endl;
		cout << "lon: " << key_value.second->getLon() << endl;
		cout << "alt: " << key_value.second->getAlt() << endl;
		cout << "classification: " << key_value.second->getClassification() << endl;
		cout << "speed: " << key_value.second->getSpeed() << endl;
		cout << "bearing: " << key_value.second->getBearing() << endl;
		cout << "***********" << endl;
	}
}
void ISRMShadow::collectDetects() {

	Detect *eoirDet1 = new Detect(0, 0, 0);
	eoirDet1->setClassification("tank");
	eoirDet1->setConfidence(0.7);


	Detect *rdrDet1 = new Detect(0.01, 0.01, 0.01);
	Detect *det2 = new Detect(1, 1, 1);
	rdrDet1->setConfidence(0.8);

	det2->setConfidence(0.9);
	det2->setClassification("bird");

	//detects.add("det1", eoirDet1);
	//detects.add("det2", rdrDet1);
	//detects.add("det3", det2);
	print();
	cout << "\n-------CORRELATING-------\n" << endl;
	correlate("det1", "det2");
	correlate("det2", "det3");
	correlate("det1", "det3");
	print();
}

void ISRMShadow::appendMoreData() {
	if (detects.contains("det2")) {
		Detect *detect = detects.get("det2");
		detect->setSpeed(1.2);
		detect->setBearing(0.7);
	}
}
void ISRMShadow::correlate(const string& id1, const string& id2) {
	if (detects.contains(id1) && detects.contains(id2)) {
		Detect *d1 = detects.get(id1);
		Detect *d2 = detects.get(id2);
		if ((std::ceil(d2->getLon() - 0.01 * 100.0) / 100.0) ==
			d1->getLon()) {

			d2->setClassification(d1->getClassification());
			detects.remove(id1);
		}
	}
}

void ISRMShadow::handleDetectionsRequest(json j) {
	//return detections by adding properties to det here
	string phase = j["phase"];
	json det;
cout << "@@@ requestRDRDetections " << j.dump(2) << endl;
cout << "@@@ requestEOIRDetections " << j.dump(2) << endl;
	amq.publish("requestRDRDetections", j, true);
	amq.publish("requestEOIRDetections", j, true);

	while (!rdrDataCollected || !eoirDataCollected) {
		Utils::sleep_for(500);
	}
	eoirDataCollected = false;
	rdrDataCollected = false;
cout << "@@@ recieveISRMDetections " << Utils::getDetectionsJson(detects).dump(2) << endl;
	amq.publish("recieveISRMDetections", Utils::getDetectionsJson(detects), true);
}

void ISRMShadow::handleRecieveRDRDetections(json j) {
	rdrDataCollected = true;
	Utils::addDetections(detects, j);
}

void ISRMShadow::handleRecieveEOIRDetections(json j) {
	eoirDataCollected = true;
	Utils::addDetections(detects, j);
}


void ISRMShadow::updateMissionPlan(const json &j) {
	MissionPlan *plan = Utils::parsePlan(j);
	planManager.add(plan->getId(), plan);
}