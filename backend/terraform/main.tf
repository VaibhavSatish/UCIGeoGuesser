terraform {
  required_providers {
    oci = {
      source = "hashicorp/oci"
    }
  }
}

provider "oci" {
  region              = var.region
  auth                = "SecurityToken"
  config_file_profile = var.config_file_profile
}

import {
  to = oci_core_instance.instance
  id = var.instance_id
}


