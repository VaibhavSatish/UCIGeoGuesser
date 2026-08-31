variable "oci_compartment_id" {
  description = "OCID from your tenancy page"
  type        = string
}
variable "oci_config_file_profile" {
  description = "profile name from your config file"
  type        = string
}

variable "oci_region" {
  description = "region where you have OCI tenancy"
  type        = string
  default     = "us-sanjose-1"
}

variable "oci_instance_id" {
  description = "OCID of the instance to import"
  type        = string
}

variable "oci_subnet_id" {
  description = "OCID of the subnet where the instance is located"
  type        = string
}

variable "oci_source_id" {
  description = "OCID of the source image for the instance"
  type        = string
}

variable "oci_ssh_public_key" {
  description = "SSH public key for the instance"
  type        = string
}

variable "gcp_project_id" {
  description = "GCP project ID"
  type        = string
}

variable "gcp_region" {
  description = "GCP region"
  type        = string
  default     = "us-west1"
}

variable "gcp_bucket_name" {
  description = "GCP bucket name"
  type        = string
}


variable "email" {
  description = "Email address for the instance"
  type        = string
}