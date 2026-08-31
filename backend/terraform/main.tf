terraform {
  required_providers {
    oci = {
      source = "oracle/oci"
    }
    google = {
      source  = "hashicorp/google"
      version = "6.8.0"
    }
  }
}

provider "oci" {
  region              = var.oci_region
  auth                = "SecurityToken"
  config_file_profile = var.oci_config_file_profile
}

provider "google" {
  project = var.gcp_project_id
  region  = var.gcp_region
}

import {
  to = oci_core_instance.instance
  id = var.oci_instance_id
}

import {
  to = google_storage_bucket.images
  id = "${var.gcp_project_id}/${var.gcp_bucket_name}"
}


