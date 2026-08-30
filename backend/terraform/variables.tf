variable "compartment_id" {
  description = "OCID from your tenancy page"
  type        = string
}
variable "config_file_profile" {
  description = "profile name from your config file"
  type        = string
}

variable "region" {
  description = "region where you have OCI tenancy"
  type        = string
  default     = "us-sanjose-1"
}

variable "instance_id" {
  description = "OCID of the instance to import"
  type        = string
}
