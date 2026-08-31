# __generated__ by Terraform
# Please review these resources and move them into your main configuration files.
# Modified by anivcs to protect sensitive information. See variables.tf and terraform.tfvars (gitignored) for the original values.


# __generated__ by Terraform from google_storage_bucket.images
resource "google_storage_bucket" "images" {
  default_event_based_hold    = false
  enable_object_retention     = false
  force_destroy               = false
  labels                      = {}
  location                    = "US-WEST1"
  name                        = var.gcp_bucket_name
  project                     = var.gcp_project_id
  public_access_prevention    = "inherited"
  requester_pays              = false
  storage_class               = "STANDARD"
  uniform_bucket_level_access = true
  soft_delete_policy {
    retention_duration_seconds = 0
  }
}
